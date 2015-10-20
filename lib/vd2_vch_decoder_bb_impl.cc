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
#include "vd2_vch_decoder_bb_impl.h"

namespace gr {
  namespace ysf {

    vd2_vch_decoder_bb::sptr
    vd2_vch_decoder_bb::make()
    {
      return gnuradio::get_initial_sptr
        (new vd2_vch_decoder_bb_impl());
    }

    vd2_vch_decoder_bb_impl::vd2_vch_decoder_bb_impl()
      : gr::block("vd2_vch_decoder_bb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {
        d_bit_counter = 0;
        d_one_voter = 0;
    }


    vd2_vch_decoder_bb_impl::~vd2_vch_decoder_bb_impl()
    {
    }

    void
    vd2_vch_decoder_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */ //FIXME issue #14
    }

    int
    vd2_vch_decoder_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char*) input_items[0];
        char *out = (char *) output_items[0];
        int output_counter = 0;

        // FIXME if i > 1 error

        int i=0;
        while(i < ninput_items[0] && output_counter < noutput_items)
        {
            // According to Figure 4-32, the first 27 bits are tripled up to 81
            // bits during TX. We use majority vote to figure out what that bit
            // was.
            if ( d_bit_counter < 81 )
            {
                d_one_voter += in[i++];
                d_bit_counter++;
                if((d_bit_counter) % 3)
                {
                    continue;
                }
                out[output_counter++] = (d_one_voter > 1);
                d_one_voter = 0;
            }
            else if ( d_bit_counter < 103)
            {
                // The remaining 22 bits are loaded as-is.
                out[output_counter++] = in[i++];
                d_bit_counter++;
            }
            else
            {
                // FIXME: check padding bit (issue #9).
                // The padding bit is silently consumed here.
                i++;
                d_bit_counter = 0;
            }
        }

        consume_each (i);
        return output_counter;
    }

  } /* namespace ysf */
} /* namespace gr */

