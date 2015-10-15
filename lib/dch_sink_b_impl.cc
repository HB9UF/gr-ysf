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
#include "dch_sink_b_impl.h"

namespace gr {
namespace ysf {

dch_sink_b::sptr
dch_sink_b::make()
{
  return gnuradio::get_initial_sptr
    (new dch_sink_b_impl());
}

dch_sink_b_impl::dch_sink_b_impl()
  : gr::block("dch_sink_b",
          gr::io_signature::make(1, 1, sizeof(char)),
          gr::io_signature::make(0, 0, 0))
{
    // Set the size of every packet. This corresponds to Table 4-2 ("Call sign
    // information of an amateur radio station") of the spec. Having the proper
    // size here is important as we'll use the size() method extensively e.g.
    // while filling.
    d_destination_packet.resize(10);
    d_source_packet.resize(10);

    d_downlink_packet.resize(10);
    d_uplink_packet.resize(10);

    d_downlink_id_packet.resize(5);
    d_uplink_id_packet.resize(5);
    d_voip_id_packet.resize(5);
    d_radio_id_packet.resize(5);

    d_byte_counter = 0;
    d_bit_counter = 7;
    d_current_packet = 0;
}

dch_sink_b_impl::~dch_sink_b_impl()
{
}

void
dch_sink_b_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
    // FIXME this will go away as we deal with issue #14.
    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
}

// FIXME: This should be implemented as sync block.
int
dch_sink_b_impl::general_work (int noutput_items,
                   gr_vector_int &ninput_items,
                   gr_vector_const_void_star &input_items,
                   gr_vector_void_star &output_items)
    {
        const char *in = (const char*) input_items[0];
        std::vector<tag_t> tags;
        uint64_t abs_offset = nitems_read(0);
        get_tags_in_range(tags, 0, abs_offset, abs_offset+ninput_items[0]);
        std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
        // This iterator will get incremented whenever we see a tag. Since we
        // sorted, we can lienarly sweep through the vector as we work through
        // the input buffer.
        std::vector<tag_t>::iterator tag = tags.begin();

        for(int i=0; i<ninput_items[0]; i++)
        {
            if( tag < tags.end() && abs_offset + i == (*tag).offset )
            {
                // We found a tag and set the current packet-pointer to the
                // corresponding packet buffer.
                d_current_tag = (demux_planner::unit_type_t)pmt::to_long((*tag).value);
                switch(d_current_tag)
                {
                    case demux_planner::CSD1:
                    case demux_planner::DEST:
                        d_current_packet = &d_destination_packet;
                        break;
                    case demux_planner::CSD2:
                    case demux_planner::DOWN:
                        d_current_packet = &d_downlink_packet;
                        break;
                    case demux_planner::SRC:
                        d_current_packet = &d_source_packet;
                        break;
                    case demux_planner::UP:
                        d_current_packet = &d_uplink_packet;
                        break;
                    case demux_planner::REM12:
                        d_current_packet = &d_downlink_id_packet;
                        break;
                    case demux_planner::REM34:
                        d_current_packet = &d_voip_id_packet;
                        break;
                }
                d_byte_counter = 0; // FIXME: This should always be the case anyway
                tag++;
            }

            if(! d_current_packet )
            {
                // FIXME this is the CRC plus some padding that we should
                // somehow deal with in a different way... For now we just
                // consume it.
                continue;
            }
            if(d_bit_counter == 7) d_current_packet->at(d_byte_counter) = 0;
            d_current_packet->at(d_byte_counter) |= ((in[i] & 0x01) << d_bit_counter);
            //std::cout << int(d_byte_counter) << "\t" << int(d_bit_counter) << "\t" << std::hex << int(in[i]) << "\t" << std::hex << d_current_packet << std::endl; FIXME
            d_bit_counter--;
            if( d_bit_counter < 0)
            {
                d_bit_counter = 7;
                d_byte_counter++;
            }
            if(d_current_packet->size() == d_byte_counter)
            {
                dump_packet();
                d_byte_counter = 0;
                // Two fields are sent back to back in a few modes. There won't
                // be any tags in those cases, hence we transition manually.
                // This is a bit of a FSM.
                if(d_current_tag == demux_planner::CSD1 && d_current_packet == &d_destination_packet) 
                {
                    d_current_packet = &d_source_packet;
                }
                else if(d_current_tag == demux_planner::CSD2 && d_current_packet == &d_downlink_packet) 
                {
                    d_current_packet = &d_uplink_packet;
                }
                // FIXME: CSD3 (see issue #17)
                else if(d_current_tag == demux_planner::REM12 && d_current_packet == &d_downlink_id_packet)
                {
                    d_current_packet = &d_uplink_id_packet;
                }
                else if(d_current_tag == demux_planner::REM34 && d_current_packet == &d_voip_id_packet)
                {
                    d_current_packet = &d_radio_id_packet;
                }
                else
                {
                    d_current_packet = 0;
                }
            }
        }
        consume_each (ninput_items[0]);
        return 0;
    }

    void
    dch_sink_b_impl::dump_packet()
    {
        std::printf("DCH packet dump:\n");
        std::printf("  [");
        for(uint8_t i=0; i<d_current_packet->size(); i++) std::printf(" %02x", d_current_packet->at(i));
        std::printf(" ]\n");
        if ( d_current_packet == &d_destination_packet)
        {
            std::printf("    Destination: ");
        }
        else if (d_current_packet == &d_source_packet)
        {
            std::printf("    Source:      ");
        }
        else if (d_current_packet == &d_downlink_packet)
        {
            std::printf("    Downlink: ");
        }
        else if (d_current_packet == &d_uplink_packet)
        {
            std::printf("    Uplink:   ");
        }
        else if (d_current_packet == &d_downlink_id_packet)
        {
            std::printf("    Downlink ID: ");
        }
        else if (d_current_packet == &d_uplink_id_packet)
        {
            std::printf("    Uplink ID:   ");
        }
        else if (d_current_packet  == &d_voip_id_packet)
        {
            std::printf("    VoIP ID:     ");
        }
        else if (d_current_packet == &d_radio_id_packet)
        {
            std::printf("    Radio ID:    ");
        }
        for(uint8_t i=0; i<d_current_packet->size(); i++) std::printf("%c", d_current_packet->at(i));
        std::printf("\n");
        std::printf("\n");
    }

  } /* namespace ysf */
} /* namespace gr */

