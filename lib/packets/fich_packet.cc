#include <packets/fich_packet.h>
#include <cstdio>

fich_packet::fich_packet()
{
    bit_counter = 7;
    byte_counter = 0;
    packet[byte_counter] = 0;
}

// FIXME This needs major refactoring (issue #21)
fich_packet::payload_type_t fich_packet::append_bit(uint8_t bit)
{
    payload_type_t ret = NONE;
    // FIXME if bit > 1 error
    packet[byte_counter] |= (bit << bit_counter);
    bit_counter = (bit_counter) ? (bit_counter-1) : 7;
    if(bit_counter == 7)
    {
        byte_counter++;
        if(byte_counter == PACKET_SIZE)
        {
            // packet full
            std::printf("FICH packet dump:\n");
            std::printf("  [");
            for(uint8_t i=0; i<PACKET_SIZE; i++) std::printf(" %02x", packet[i]);
            std::printf(" ]\n");
            if(get_fi() == 0x00) ret = HEADER;
            else if(get_fi() == 0x02) ret = TERMINATOR;
            else if(get_fi() == 0x01 && get_dt() == 0x02)
            {
                switch(get_fn())
                {
                    case 0:
                        ret = VD2_0;
                        break;
                    case 1:
                        ret = VD2_1;
                        break;
                    case 2:
                        ret = VD2_2;
                        break;
                    case 3:
                        ret = VD2_3;
                        break;
                    case 4:
                        ret = VD2_4;
                        break;
                    case 5:
                        ret = VD2_5;
                        break;
                    case 6:
                        ret = VD2_6;
                        break;
                    case 7:
                        ret = VD2_7;
                        break;
                }
            }
            else ret = FIXME;
            std::printf("    FI: %02x\n", get_fi());
            std::printf("    CS: %02x\n", get_cs());
            std::printf("    CM: %02x\n", get_cm());
            std::printf("    BN: %02x\n", get_bn());
            std::printf("    BT: %02x\n", get_bt());
            std::printf("    FN: %02x\n", get_fn());
            std::printf("    FT: %02x\n", get_ft());
            std::printf("    MR: %02x\n", get_mr());
            std::printf("    VP: %02x\n", get_vp());
            std::printf("    DT: %02x\n", get_dt());
            std::printf("    SQ: %02x\n", get_sq());
            std::printf("    SC: %02x\n", get_sc());
            std::printf("\n");
            byte_counter = 0;
        }
        packet[byte_counter] = 0;
    }
    return ret;

}

// Bitmasks as explained in Table 4-1 of the spec.
uint8_t fich_packet::get_fi()
{
    return (packet[0] >> 6) & 0x03;
}

uint8_t fich_packet::get_cs()
{
    return (packet[0] >> 4) & 0x03;
}

uint8_t fich_packet::get_cm()
{
    return (packet[0] >> 2) & 0x03;
}

uint8_t fich_packet::get_bn()
{
    return (packet[0] >> 0) & 0x03;
}

uint8_t fich_packet::get_bt()
{
    return (packet[1] >> 6) & 0x03;
}

uint8_t fich_packet::get_fn()
{
    return (packet[1] >> 3) & 0x07;
}

uint8_t fich_packet::get_ft()
{
    return (packet[1] >> 0) & 0x07;
}

uint8_t fich_packet::get_mr()
{
    return (packet[2] >> 3) & 0x07;
}

uint8_t fich_packet::get_vp()
{
    return (packet[2] >> 2) & 0x01;
}

uint8_t fich_packet::get_dt()
{
    return (packet[2] >> 0) & 0x03;
}

uint8_t fich_packet::get_sq()
{
    return (packet[3] >> 7) & 0x01;
}

uint8_t fich_packet::get_sc()
{
    return (packet[3] >> 0) & 0x3f;
}
