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

#ifndef INCLUDED_YSF_MBELIB_BLOCK_BF_IMPL_H
#define INCLUDED_YSF_MBELIB_BLOCK_BF_IMPL_H

#include <ysf/mbelib_block_bf.h>
#include <cstdint>

namespace gr {
  namespace ysf {

extern "C"
{
    #include <mbelib.h>
}
    class mbelib_block_bf_impl : public mbelib_block_bf
    {
     private:
      mbe_parms d_cur_mp;
      mbe_parms d_prev_mp;
      mbe_parms d_prev_mp_enhanced;

      enum codec_t { AMBE3600X2400, AMBE3600X2450, IMBE7200X4400 };
      codec_t d_codec;

      char d_mbe_packet_buffer[88]; // For AMBE we only use 49 out of the 88
      uint8_t d_mbe_packet_size;
      uint16_t d_output_packet_size;
      uint8_t d_mbe_packet_counter;

     public:
      mbelib_block_bf_impl(std::string codec);
      ~mbelib_block_bf_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ysf
} // namespace gr

#endif /* INCLUDED_YSF_MBELIB_BLOCK_BF_IMPL_H */

