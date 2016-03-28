/* -*- c++ -*- */
/* 
 * Copyright 2015 Mathias Weyland
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdbool>
#include <gnuradio/io_signature.h>
#include "deframer_bb_impl.h"

// Enumerating output streams
const int FICH_STREAM = 0;
const int VD1_DCH_STREAM = 1;
const int VD2_DCH_STREAM = 2;
const int VD_VCH_STREAM = 3;
const int FR_VCH_STREAM = 4;

namespace gr {
  namespace ysf {

    deframer_bb::sptr
    deframer_bb::make(int threshold)
    {
      return gnuradio::get_initial_sptr
        (new deframer_bb_impl(threshold));
    }

    deframer_bb_impl::deframer_bb_impl(int threshold)
      : gr::block("deframer_bb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(5, 5, sizeof(char)))
    {
        d_state = UNSYNCED;
        d_shift_register = 0;
        d_packet_dibit_counter = 0;
        // FIXME: deal with threshold (issue #20)
        
        message_port_register_in(pmt::mp("demux_instruction"));
        set_msg_handler(pmt::mp("demux_instruction"), boost::bind(&deframer_bb_impl::queue, this, _1));
    }

    deframer_bb_impl::~deframer_bb_impl()
    {
    }

    void
    deframer_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        // FIXME issue #15
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    deframer_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char*) input_items[0];
        int input_counter = 0;

        char *out_fich = (char*) output_items[FICH_STREAM];
        int output_counter_fich = 0;

        // We have a payload_streams structure here that allows us to pass
        // the buffers with the relevant information to an execute method.
        payload_streams_t payload_streams =
        {
            (char*) output_items[VD1_DCH_STREAM], // out_vd1_dch
            (char*) output_items[VD2_DCH_STREAM], // out_vd2_dch
            (char*) output_items[VD_VCH_STREAM], // out_vd_vch
            (char*) output_items[FR_VCH_STREAM], // out_fr_vch
            0, // output_counter_vd1_dch
            0, // output_counter_vd2_dch
            0, // output_counter_vd_vch
            0, // output_counter_fr_vch
            noutput_items, // noutput_item
        };

        bool flush = false; // Will break the while loop and return to the scheduler if set to true

        // When storing the packet in this block, we also keep track of the
        // packet offsets in order to be easily able to tag them when we send 
        uint64_t block_offset = nitems_read(0);
        while(input_counter < ninput_items[0] && output_counter_fich < noutput_items && !flush)
        {
            // FIXME if(in[input_counter] > 3) error;
            switch( d_state )
            {
                case UNSYNCED:
                    d_shift_register = (d_shift_register << 2) | (in[input_counter++] & 0b11);
                    if( (d_shift_register & 0xffffffffff) == 0xd471c9634d )
                    {
                        // We do this early because if we arm *after* the
                        // message has arrived, the planner will wait for
                        // something that has arrived already.
                        d_planner.arm(); 
                        d_state = FICH;
                        d_packet_dibit_counter = 0;
                    }
                    break;

                case FICH:
                    d_packet_dibit_counter++;
                    out_fich[output_counter_fich++] = in[input_counter++];
                    if( d_packet_dibit_counter == 100 )
                    {
                        // If we don't flush here, we'll be stuck in a deadlock.
                        // The fich_sink will never send the demux instruction
                        // because the fich package will be waiting in this
                        // block's output buffer while we wait for that message.
                        flush = true;
                        d_packet_dibit_counter = 0;
                        d_state = LOAD_PAYLOAD;
                    }
                    break;

                case LOAD_PAYLOAD:
                    d_packet_buffer[d_packet_dibit_counter] = in[input_counter++];
                    d_packet_dibit_counter++;
                    if( d_packet_dibit_counter == 360 )
                    {
                        d_state = FORWARD_PAYLOAD;
                    }
                    break;

                case FORWARD_PAYLOAD:
                    switch( d_planner.get_state() )
                    {
                        case demux_planner::STALL:
                            // When stalling, we have to return control to the
                            // scheduler such that it manages to deliver the
                            // message we're waiting on here.
                            flush = true;
                            break;
                        case demux_planner::ACTIONS_PENDING:
                            fill_output_buffers(payload_streams);
                            break;
                        case demux_planner::DONE:
                            d_state = UNSYNCED;
                            break;
                    }
                    break;
            }
        }

        consume_each (input_counter);
        produce(FICH_STREAM, output_counter_fich);
        produce(VD1_DCH_STREAM, payload_streams.output_counter_vd1_dch);
        produce(VD2_DCH_STREAM, payload_streams.output_counter_vd2_dch);
        produce(VD_VCH_STREAM, payload_streams.output_counter_vd_vch);
        produce(FR_VCH_STREAM, payload_streams.output_counter_fr_vch);

        return WORK_CALLED_PRODUCE;
    }

    void
    deframer_bb_impl::fill_output_buffers(payload_streams_t &payload_streams)
    {
        while( d_planner.get_state() == demux_planner::ACTIONS_PENDING && 
            payload_streams.output_counter_vd1_dch < payload_streams.noutput_items &&
            payload_streams.output_counter_vd2_dch < payload_streams.noutput_items &&
            payload_streams.output_counter_vd_vch  < payload_streams.noutput_items &&
            payload_streams.output_counter_fr_vch  < payload_streams.noutput_items ) 
        {
            demux_planner::plan_item_t item = d_planner.get_current_item();
            switch( item.output_stream )
            {
                case demux_planner::VD1_DCH:
                    if(item.tag != demux_planner::NONE)
                    {
                        add_item_tag(VD1_DCH_STREAM, nitems_written(VD1_DCH_STREAM)+payload_streams.output_counter_vd1_dch, pmt::string_to_symbol("type"), pmt::mp(item.tag));
                    }
                    else
                    {
                        payload_streams.out_vd1_dch[payload_streams.output_counter_vd1_dch++] =
                            d_packet_buffer[item.position];
                    }
                    break;
                case demux_planner::VD2_DCH:
                    if(item.tag != demux_planner::NONE)
                    {
                        add_item_tag(VD2_DCH_STREAM, nitems_written(VD2_DCH_STREAM)+payload_streams.output_counter_vd2_dch, pmt::string_to_symbol("type"), pmt::mp(item.tag));
                    }
                    else
                    {
                        payload_streams.out_vd2_dch[payload_streams.output_counter_vd2_dch++] =
                            d_packet_buffer[item.position];
                    }
                    break;
                case demux_planner::VD_VCH:
                    payload_streams.out_vd_vch[payload_streams.output_counter_vd_vch++] =
                        d_packet_buffer[item.position];
                    break;
                case demux_planner::FR_VCH:
                    payload_streams.out_fr_vch[payload_streams.output_counter_fr_vch++] =
                        d_packet_buffer[item.position];
                    break;

            }
            d_planner.consume_current_item();
        }
    }

    void
    deframer_bb_impl::queue (pmt::pmt_t msg)
    {
        long fi = pmt::to_long(pmt::dict_ref(msg, pmt::string_to_symbol("FI"), pmt::PMT_NIL));
        long fn = pmt::to_long(pmt::dict_ref(msg, pmt::string_to_symbol("FN"), pmt::PMT_NIL));
        long dt = pmt::to_long(pmt::dict_ref(msg, pmt::string_to_symbol("DT"), pmt::PMT_NIL));

        d_planner.plan_for(fi, fn, dt);
    }
  } /* namespace ysf */
} /* namespace gr */

