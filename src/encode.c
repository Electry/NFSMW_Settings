#include <vitasdk.h>
#include <taihen.h>

#include "main.h"

/*
 * T1 MOVS <Rd>,#<imm8>    # Outside IT block.
 *    MOV<c> <Rd>,#<imm8>  # Inside IT block.
 *
 * 001            00     xxx xxxxxxxx
 * Move immediate OPcode Rdn imm8
 *
 * byte 1   byte 0
 * 00100xxx xxxxxxxx
 */
uint8_t *encode_t1_mov(uint8_t reg, uint8_t value) {
    static uint8_t out[2];
    memset(out, 0, 2);

    out[1] |= 0b00100000;   // Move immediate
    out[1] |= reg;          // Rd
    out[0] |= value;
    return out;
}

/*
 * T2 MOV{S}<c>.W <Rd>,#<const>
 *
 * 11110           x 0      0010   x 1111 - 0  xxx  xxxx xxxxxxxx
 * Data processing i 12-bit OPcode S Rn     DP imm3 Rd   imm8
 *
 * byte 1   byte 0     byte 3   byte 2
 * 11110x00 010x1111 - 0xxxxxxx xxxxxxxx
 */
uint8_t *encode_t2_mov(bool setflags, uint8_t reg, uint32_t value) {
    static uint8_t out[4];
    memset(out, 0, 4);

    out[1] |= 0b11110000;       // Data processing (12-bit)
    out[0] |= 0b01000000;       // OPcode
    out[0] |= 0b00001111;       // Rn
    if (setflags)
        out[0] |= 0b00010000;   // S
    out[3] |= reg; // Rd

    uint16_t imm12 = 0;

    if (value < 256) {
        imm12 = value;
    } else {
        uint32_t tmp = value;
        uint8_t msb = 0;
        while (tmp != 1 && ++msb) {
            tmp = tmp >> 1;
        }
        imm12 = (value >> (msb - 7)) & 0b000001111111; // rotated value as bit[6:0]
        imm12 |= (32 + 7 - msb) << 7; // rotation as bit[11:7]
    }

    out[1] |= (imm12 & 0b100000000000) >> 9;    // i
    out[3] |= (imm12 & 0b011100000000) >> 4;    // imm3
    out[2] |= (imm12 & 0b000011111111);         // imm8
    return out;
}

/*
 * T3 MOVW<c> <Rd>,#<imm16>
 *
 * 11110           x 10     0  1    00  xxxx - 0  xxx  xxxx xxxxxxxx
 * Data processing i 16-bit OP Move OP2 imm4   DP imm3 Rd   imm8
 *
 * byte 1   byte 0     byte 3   byte 2
 * 11110x10 0100xxxx - 0xxxxxxx xxxxxxxx
 */
uint8_t *encode_t3_mov(uint8_t reg, uint16_t value) {
    static uint8_t out[4];
    memset(out, 0, 4);

    out[1] |= 0b11110010; // Data processing (16-bit)
    out[0] |= 0b01000000; // Move, plain (16-bit)
    out[3] |= reg;        // Rd

    out[0] |= (value & 0b1111000000000000) >> 12;   // imm4
    out[1] |= (value & 0b0000100000000000) >> 9;    // i
    out[3] |= (value & 0b0000011100000000) >> 4;    // imm3
    out[2] |= (value & 0b0000000011111111);         // imm8
    return out;
}

/*
 * T1 MOVT<c> <Rd>,#<imm16>
 *
 * 11110           x 10     1  1    00  xxxx - 0  xxx  xxxx xxxxxxxx
 * Data processing i 16-bit OP Move OP2 imm4   DP imm3 Rd   imm8
 *
 * byte 1   byte 0     byte 3   byte 2
 * 11110x10 1100xxxx - 0xxxxxxx xxxxxxxx
 */
uint8_t *encode_t1_movt(uint8_t reg, uint16_t value) {
    static uint8_t out[4];
    memset(out, 0, 4);

    out[1] |= 0b11110010; // Data processing (16-bit)
    out[0] |= 0b11000000; // Move top, plain (16-bit)
    out[3] |= reg;        // Rd

    out[0] |= (value & 0b1111000000000000) >> 12;   // imm4
    out[1] |= (value & 0b0000100000000000) >> 9;    // i
    out[3] |= (value & 0b0000011100000000) >> 4;    // imm3
    out[2] |= (value & 0b0000000011111111);         // imm8
    return out;
}

uint8_t *encode_mov32(uint8_t reg, uint32_t value) {
    static uint8_t out[8];

    memcpy(&out[0], encode_t3_mov(reg, value), 4);
    memcpy(&out[4], encode_t1_movt(reg, value >> 16), 4);

    return out;
}

uint8_t *encode_t2_vmov_f32(uint8_t reg, uint32_t value) {
    static uint8_t out[4];
    memset(out, 0, 4);

    out[1] |= 0b11101110; // Condition + OP
    out[0] |= 0b10110000; // OP
    out[3] |= 0b00001010; // OP

    // Vd:D
    if (reg & 0b1)
        out[0] |= 0b01000000;       // D
    out[3] |= (reg & 0b11110) << 3; // Vd

    // imm4H:imm4L
    uint8_t imm8 = 0;
    if (value & (1 << 31))
        imm8 |= 1 << 7; // sign bit
    if (((value >> 25) & 0b111111) < 0b100000)
        imm8 |= 1 << 6; // < 2.0
    imm8 |= (value >> 19) & 0b111111;

    out[0] |= (imm8 & 0b11110000) >> 4; // imm4H
    out[2] |= (imm8 & 0b00001111);      // imm4L
    return out;
}
