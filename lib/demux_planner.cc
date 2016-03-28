#include "demux_planner.h"
#include <iostream>

demux_planner::demux_planner()
{
    d_state = DONE;
}

void
demux_planner::arm()
{
    d_state = STALL;
}

demux_planner::state_t
demux_planner::get_state()
{
    return d_state;
}

demux_planner::plan_item_t
demux_planner::get_current_item()
{
    // FIXME: !d_item_queue.empty()
    return d_item_queue.front();
}


// FIXME: Merge with get_current_item()
void
demux_planner::consume_current_item()
{
    // FIXME: !d_item_queue.empty()
    plan_item_t i = d_item_queue.front();
    if(i.position == i.end-1)
    {
        d_item_queue.pop();
    }
    else
    {
        d_item_queue.front().position += 1;
        if(i.tag != NONE)
            d_item_queue.front().tag = NONE;
    }
    d_state = ( d_item_queue.empty() ) ? DONE : ACTIONS_PENDING;
}

void
demux_planner::plan_for(long fi, long fn, long dt)
{
    // Number of bits are always divided by two as we are doing this for 
    // dibit positions
    switch(fi)
    {
        // Header Channel or Terminator Channel, respectively
        case 0b00:
        case 0b10:
            // De-interleave the two DCH packets. See Figure 4-7 of the spec.
            d_item_queue.push({ VD1_DCH, 0, 1, CSD1 } );
            for(uint8_t i=0; i < 10; i+=2)
                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
            d_item_queue.push({ VD1_DCH, 0, 1, CSD2 } );
            for(uint8_t i=1; i < 10; i+=2)
                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
            break;

        // Communication Channel
        case 0b01:
            switch(dt)
            {
                // V/D mode type 2
                case 0b10:
                    // First set a tag at the beginning of the packet ...
                    switch (fn)
                    {
                        case 0:
                            d_item_queue.push({ VD2_DCH, 0, 1, DEST } );
                            break;
                        case 1:
                            d_item_queue.push({ VD2_DCH, 0, 1, SRC } );
                            break;
                        case 2:
                            d_item_queue.push({ VD2_DCH, 0, 1, DOWN } );
                            break;
                        case 3:
                            d_item_queue.push({ VD2_DCH, 0, 1, UP } );
                            break;
                        case 4:
                            d_item_queue.push({ VD2_DCH, 0, 1, REM12 } );
                            break;
                        case 5:
                            d_item_queue.push({ VD2_DCH, 0, 1, REM34 } );
                            break;
                    }
                    // .. then de-interleave according to Figure 4-12
                    for(uint8_t i=0; i < 5; i++)
                    {
                        d_item_queue.push({ VD2_DCH, i*(144/2), i*(144/2)+40/2, NONE } );
                        d_item_queue.push({ VD_VCH, 40/2+i*(144/2), 40/2+i*(144/2)+104/2, NONE } );
                    }
                    break;
                // Data FR mode
                case 0b01:
                    switch (fn)
                    {
                        case 0:
                            d_item_queue.push({ VD1_DCH, 0, 1, CSD1 } );
                            for(uint8_t i=0; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            d_item_queue.push({ VD1_DCH, 0, 1, CSD2 } );
                            for(uint8_t i=1; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            break;
                        case 1:
                            d_item_queue.push({ VD1_DCH, 0, 1, CSD3 } );
                            for(uint8_t i=0; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            d_item_queue.push({ VD1_DCH, 0, 1, DT } );
                            for(uint8_t i=1; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            break;
                        default:
                            d_item_queue.push({ VD1_DCH, 0, 1, DT } );
                            for(uint8_t i=0; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            d_item_queue.push({ VD1_DCH, 0, 1, DT } );
                            for(uint8_t i=1; i < 10; i+=2)
                                d_item_queue.push({ VD1_DCH, i*(72/2), i*(72/2)+72/2, NONE } );
                            break;
                    }
                    break;

                case 0b11:
                    for(uint8_t i=0; i < 5; i++)
                        d_item_queue.push({ FR_VCH, i*(144/2), i*(144/2)+144/2, NONE } );
                    break;

                default:
                    std::cerr << "Ignoring unknown packet with DT=" << dt << std::endl;
                    break;
            }
            break;
        default:
                // We don't add anything here. It's going to go straight back
                // to UNSYNCED which is what we want.
            std::cerr << "Ignoring unknown packet with FI=" << fi << std::endl;
            break;
    }
    d_state = ( d_item_queue.empty() ) ? DONE : ACTIONS_PENDING;
}
