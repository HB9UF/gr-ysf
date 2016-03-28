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

#ifndef INCLUDED_YSF_ALIGNER_BB_IMPL_H
#define INCLUDED_YSF_ALIGNER_BB_IMPL_H

#include <ysf/deframer_bb.h>

#include <cstdint>
#include "demux_planner.h"

namespace gr {
  namespace ysf {

    class deframer_bb_impl : public deframer_bb
    {
     private:
        struct payload_streams_t
        {
            char *out_vd1_dch;
            char *out_vd2_dch;
            char *out_vd_vch;
            char *out_fr_vch;
            uint16_t output_counter_vd1_dch;
            uint16_t output_counter_vd2_dch;
            uint16_t output_counter_vd_vch;
            uint16_t output_counter_fr_vch;
            const int noutput_items;
        };

        enum state_t { UNSYNCED, FICH, LOAD_PAYLOAD, FORWARD_PAYLOAD };
        state_t d_state;
        uint64_t d_shift_register;
        uint16_t d_packet_dibit_counter;
        uint8_t d_packet_buffer[360];
        demux_planner d_planner;
        void queue(pmt::pmt_t msg);
        void fill_output_buffers(payload_streams_t &payload_streams);

     public:
      deframer_bb_impl(int threshold);
      ~deframer_bb_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ysf
} // namespace gr

#endif /* INCLUDED_YSF_ALIGNER_BB_IMPL_H */

