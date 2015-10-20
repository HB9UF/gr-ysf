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
#include <volk/volk.h>
#include "mbelib_block_bf_impl.h"

namespace gr {
  namespace ysf {

    mbelib_block_bf::sptr
    mbelib_block_bf::make(std::string codec)
    {
      return gnuradio::get_initial_sptr
        (new mbelib_block_bf_impl(codec));
    }
    
    mbelib_block_bf_impl::mbelib_block_bf_impl(std::string codec)
      : gr::block("mbelib_block_bf",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(float)))
    {
        d_mbe_packet_counter = 0;
        mbe_initMbeParms(&d_cur_mp, &d_prev_mp, &d_prev_mp_enhanced);
    }

    mbelib_block_bf_impl::~mbelib_block_bf_impl()
    {
    }

    void
    mbelib_block_bf_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        // FIXME implement this
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int
    mbelib_block_bf_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        float *out = (float*) output_items[0];

        int input_counter = 0;
        int output_counter = 0;

        while(input_counter < ninput_items[0] && output_counter + 160 < noutput_items)
        {
            d_mbe_packet_buffer[d_mbe_packet_counter++] = in[input_counter++];
            static int tmp = 1;
            if(d_mbe_packet_counter == 49)
            {
                d_mbe_packet_counter = 0;
                // FIXME: Deal with those
                int errs = 0;
                int errs2 = 0;
                char err_str[64];

                float audio_out_buf[160];
                // FIXME select vocoder here
                mbe_processAmbe2450Dataf(audio_out_buf, &errs, &errs2, err_str, d_mbe_packet_buffer, &d_cur_mp, &d_prev_mp, &d_prev_mp_enhanced, 3);
                volk_32f_s32f_multiply_32f(out+output_counter, audio_out_buf, 1./32768, 160);
                output_counter += 160;
            }
        }

        consume_each (input_counter);

        return output_counter;
    }

  } /* namespace ysf */
} /* namespace gr */

