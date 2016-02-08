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

#include <gnuradio/io_signature.h>
#include "fich_sink_b_impl.h"
#include "demux_planner.h"

namespace gr {
  namespace ysf {

    fich_sink_b::sptr
    fich_sink_b::make()
    {
      return gnuradio::get_initial_sptr
        (new fich_sink_b_impl());
    }

    fich_sink_b_impl::fich_sink_b_impl()
      : gr::block("fich_sink_b",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, sizeof(char))) // FIXME 0? Address with issue #14
    {
        message_port_register_out(pmt::mp("demux_instruction"));
    }

    fich_sink_b_impl::~fich_sink_b_impl()
    {
    }

    void
    fich_sink_b_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        // FIXME will be fixed with issue #14
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    fich_sink_b_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char*) input_items[0];
        for(int i=0; i<ninput_items[0]; i++)
        {
            bool full = packet.append_bit(in[i]);
            if(full)
            {
                message_port_pub(pmt::string_to_symbol("demux_instruction"), pmt::cons(pmt::mp(packet.get_fi()), pmt::mp(packet.get_fn())));
            }
        }
        consume_each (ninput_items[0]);
        return 0;
    }

  } /* namespace ysf */
} /* namespace gr */

