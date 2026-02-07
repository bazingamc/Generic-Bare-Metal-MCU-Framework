#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ENDIAN_LITTLE = 0,
    ENDIAN_BIG    = 1,
} EndianType;

uint16_t endian_swap16_u(uint16_t v);
int16_t  endian_swap16_s(int16_t v);

uint32_t endian_swap32_u(uint32_t v);
int32_t  endian_swap32_s(int32_t v);

float    endian_swap_float(float v);
double   endian_swap_double(double v);

uint32_t endian_parse_u32(const uint8_t* buf, uint8_t len, EndianType endian);
void     endian_write_u32(uint8_t* dst, uint8_t len, EndianType endian, uint32_t v);
void     endian_write_bytes(uint8_t* dst, const uint8_t* src, uint8_t len, EndianType endian);

#ifdef __cplusplus
}
#endif
