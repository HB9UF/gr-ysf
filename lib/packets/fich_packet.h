#ifndef _FICH_PACKET_H
#define _FICH_PACKET_H

#define PACKET_SIZE 6

#include <cstdint>

class fich_packet
{
    public:
        fich_packet();
        bool append_bit(uint8_t bit);

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

    private:
        uint8_t bit_counter;
        uint8_t byte_counter;
        uint8_t packet[PACKET_SIZE];
        // Stores old packet double-buffer style for getters
        uint8_t shadow_packet[PACKET_SIZE]; 


};

#endif
