#ifndef _FICH_PACKET_H
#define _FICH_PACKET_H

#define PACKET_SIZE 6

#include <cstdint>

class fich_packet
{
    public:
        fich_packet();
        enum payload_type_t {NONE, HEADER, TERMINATOR, VD2_0, VD2_1, VD2_2, VD2_3, VD2_4, VD2_5, VD2_6, VD2_7, FIXME};
        payload_type_t append_bit(uint8_t bit);

    private:
        uint8_t bit_counter;
        uint8_t byte_counter;
        uint8_t packet[PACKET_SIZE];

        // Only make those public with a double buffer (FIXME issue #21)
        uint8_t get_fi();
        uint8_t get_cs();
        uint8_t get_cm();
        uint8_t get_bn();
        uint8_t get_bt();
        uint8_t get_fn();
        uint8_t get_ft();
        uint8_t get_mr();
        uint8_t get_vp();
        uint8_t get_dt();
        uint8_t get_sq();
        uint8_t get_sc();

};

#endif
