#include "fich_packet.h"
#include <cstdio>
#include <cstring>

fich_packet::fich_packet()
{
    bit_counter = 7;
    byte_counter = 0;
    packet[byte_counter] = 0;
    // FIXME: Initialize shadow_packet?
}

bool fich_packet::append_bit(uint8_t bit)
{
    bool ret = false;
    // FIXME if bit > 1 error
    packet[byte_counter] |= (bit << bit_counter);
    bit_counter = (bit_counter) ? (bit_counter-1) : 7;
    if(bit_counter == 7)
    {
        byte_counter++;
        if(byte_counter == PACKET_SIZE)
        {
            // packet full
            ret = true;
            std::memcpy(shadow_packet, packet, PACKET_SIZE);

            std::printf("FICH packet dump:\n");
            std::printf("  [");
            for(uint8_t i=0; i<PACKET_SIZE; i++) std::printf(" %02x", packet[i]);
            std::printf(" ]\n");

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
    return (shadow_packet[0] >> 6) & 0x03;
}

uint8_t fich_packet::get_cs()
{
    return (shadow_packet[0] >> 4) & 0x03;
}

uint8_t fich_packet::get_cm()
{
    return (shadow_packet[0] >> 2) & 0x03;
}

uint8_t fich_packet::get_bn()
{
    return (shadow_packet[0] >> 0) & 0x03;
}

uint8_t fich_packet::get_bt()
{
    return (shadow_packet[1] >> 6) & 0x03;
}

uint8_t fich_packet::get_fn()
{
    return (shadow_packet[1] >> 3) & 0x07;
}

uint8_t fich_packet::get_ft()
{
    return (shadow_packet[1] >> 0) & 0x07;
}

uint8_t fich_packet::get_mr()
{
    return (shadow_packet[2] >> 3) & 0x07;
}

uint8_t fich_packet::get_vp()
{
    return (shadow_packet[2] >> 2) & 0x01;
}

uint8_t fich_packet::get_dt()
{
    return (shadow_packet[2] >> 0) & 0x03;
}

uint8_t fich_packet::get_sq()
{
    return (shadow_packet[3] >> 7) & 0x01;
}

uint8_t fich_packet::get_sc()
{
    return (shadow_packet[3] >> 0) & 0x3f;
}
