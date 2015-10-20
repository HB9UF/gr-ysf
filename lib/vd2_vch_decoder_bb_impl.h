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

#ifndef INCLUDED_YSF_VD2_VCH_DECODER_BB_IMPL_H
#define INCLUDED_YSF_VD2_VCH_DECODER_BB_IMPL_H

#include <ysf/vd2_vch_decoder_bb.h>
#include <cstdint>

namespace gr {
  namespace ysf {

    class vd2_vch_decoder_bb_impl : public vd2_vch_decoder_bb
    {
     private:
         uint16_t d_bit_counter;
         uint8_t d_one_voter;

     public:
      vd2_vch_decoder_bb_impl();
      ~vd2_vch_decoder_bb_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ysf
} // namespace gr

#endif /* INCLUDED_YSF_VD2_VCH_DECODER_BB_IMPL_H */

