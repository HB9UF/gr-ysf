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

#ifndef INCLUDED_YSF_DCH_SINK_C_IMPL_H
#define INCLUDED_YSF_DCH_SINK_C_IMPL_H

#include <ysf/dch_sink_b.h>
#include <vector>
#include "demux_planner.h"

namespace gr {
  namespace ysf {

    class dch_sink_b_impl : public dch_sink_b
    {
     private:
         uint8_t d_byte_counter; // current byte within the packet
         int8_t d_bit_counter;

         std::vector<uint8_t> d_destination_packet;
         std::vector<uint8_t> d_source_packet;

         std::vector<uint8_t> d_downlink_packet;
         std::vector<uint8_t> d_uplink_packet;

         std::vector<uint8_t> d_downlink_id_packet;
         std::vector<uint8_t> d_uplink_id_packet;

         std::vector<uint8_t> d_voip_id_packet;
         std::vector<uint8_t> d_radio_id_packet;

         // points to packet we are currently filling
         std::vector<uint8_t> *d_current_packet; 
         // Used to implement manual transitions from one packet to another.
         demux_planner::unit_type_t d_current_tag;

         void dump_packet();

     public:
      dch_sink_b_impl();
      ~dch_sink_b_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ysf
} // namespace gr

#endif /* INCLUDED_YSF_DCH_SINK_C_IMPL_H */
