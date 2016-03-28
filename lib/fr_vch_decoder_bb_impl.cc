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
#include "fr_vch_decoder_bb_impl.h"

namespace gr {
  namespace ysf {

    fr_vch_decoder_bb::sptr
    fr_vch_decoder_bb::make()
    {
      return gnuradio::get_initial_sptr
        (new fr_vch_decoder_bb_impl());
    }

    fr_vch_decoder_bb_impl::fr_vch_decoder_bb_impl()
      : gr::block("fr_vch_decoder_bb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)))
    {
        d_bit_counter = 0;
    }

    fr_vch_decoder_bb_impl::~fr_vch_decoder_bb_impl()
    {
    }

    void
    fr_vch_decoder_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */ //FIXME issue #14
    }

    int
    fr_vch_decoder_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const char *in = (const char*) input_items[0];
        char *out = (char *) output_items[0];

        int input_counter = 0;
        int output_counter = 0;
        while(input_counter < ninput_items[0])
        {

            if(d_bit_counter == 144)
            {
                if(noutput_items - output_counter < 88) break; // output buffer too full

                d_bit_counter = 0;

                uint8_t reshuffled_frame[144];
                reshuffle_frame(d_frame, reshuffled_frame);

                uint16_t seed = 0;
                for(uint16_t i=0; i<12; i++)
                {
                    seed = (seed << 1) | reshuffled_frame[i];
                }

                scramble(reshuffled_frame+23, reshuffled_frame+23, 114-23-7, seed, 4);

                for(uint8_t i=  0; i< 12; i++) out[output_counter++] = reshuffled_frame[i]; // u0
                for(uint8_t i= 23; i< 35; i++) out[output_counter++] = reshuffled_frame[i]; // u1
                for(uint8_t i= 46; i< 58; i++) out[output_counter++] = reshuffled_frame[i]; // u2
                for(uint8_t i= 69; i< 81; i++) out[output_counter++] = reshuffled_frame[i]; // u3

                for(uint8_t i= 92; i<103; i++) out[output_counter++] = reshuffled_frame[i]; // u4
                for(uint8_t i=107; i<118; i++) out[output_counter++] = reshuffled_frame[i]; // u5
                for(uint8_t i=122; i<133; i++) out[output_counter++] = reshuffled_frame[i]; // u6

                for(uint8_t i=137; i<144; i++) out[output_counter++] = reshuffled_frame[i]; // u7
            }

            d_frame[d_bit_counter++] = in[input_counter++];
        }

        consume_each(input_counter);
        produce(0, output_counter);

        return WORK_CALLED_PRODUCE;
    }

    void
    fr_vch_decoder_bb_impl::reshuffle_frame(uint8_t old_frame[144], uint8_t new_frame[144])
    {
        const uint8_t permutation[144] = {
              0,   7,  12,  19,  24,  31,  36,  43,  48,  55,  60,  67, // [  0 -  11] yellow message
             72,  79,  84,  91,  96, 103, 108, 115, 120, 127, 132,      // [ 12 -  22] yellow FEC
            139,   1,   6,  13,  18,  25,  30,  37,  42,  49,  54,  61, // [ 23 -  34] orange message
             66,  73,  78,  85,  90,  97, 102, 109, 114, 121, 126,      // [ 35 -  45] orange FEC
            133, 138,   2,   9,  14,  21,  26,  33,  38,  45,  50,  57, // [ 46 -  57] red message
             62,  69,  74,  81,  86,  93,  98, 105, 110, 117, 122,      // [ 58 -  68] red FEC
            129, 134, 141,   3,   8,  15,  20,  27,  32,  39,  44,  51, // [ 69 -  80] pink message
             56,  63,  68,  75,  80,  87,  92,  99, 104, 111, 116,      // [ 81 -  91] pink FEC
            123, 128, 135, 140,   4,  11,  16,  23,  28,  35,  40,      // [ 92 - 102] dark blue message
             47,  52,  59,  64,                                         // [103 - 106] dark blue FEC
             71,  76,  83,  88,  95, 100, 107, 112, 119, 124, 131,      // [107 - 117] light blue message
            136, 143,   5,  10,                                         // [118 - 121] light blue FEC
             17,  22,  29,  34,  41,  46,  53,  58,  65,  70,  77,      // [122 - 132] green message
             82,  89,  94, 101,                                         // [133 - 136] green FEC
            106, 113, 118, 125, 130, 137, 142,                          // [137 - 143] unprotected
        };

        for(uint8_t i=0; i<144; i++)
        {
            new_frame[i] = old_frame[permutation[i]];
        }
    }

    void
    fr_vch_decoder_bb_impl::scramble(uint8_t out[], uint8_t in[], uint16_t n, uint32_t seed, uint8_t shift)
    {
        uint32_t v = seed << shift;
        for (uint16_t i=0; i<n; i++)
        {
            v = ((v * 173) + 13849) & 0xffff;
            out[i] = in[i] ^ (v >> 15);
        }
    }

  } /* namespace ysf */
} /* namespace gr */

