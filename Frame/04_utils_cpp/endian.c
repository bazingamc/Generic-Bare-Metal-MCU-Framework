#include "endian.h"

uint16_t endian_swap16_u(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}

int16_t endian_swap16_s(int16_t v)
{
    return (int16_t)endian_swap16_u((uint16_t)v);
}

uint32_t endian_swap32_u(uint32_t v)
{
    return (v >> 24) |
           ((v >> 8)  & 0x0000FF00u) |
           ((v << 8)  & 0x00FF0000u) |
           (v << 24);
}

int32_t endian_swap32_s(int32_t v)
{
    return (int32_t)endian_swap32_u((uint32_t)v);
}

float endian_swap_float(float v)
{
    union
    {
        float   f;
        uint8_t b[4];
    } in, out;

    in.f = v;
    out.b[0] = in.b[3];
    out.b[1] = in.b[2];
    out.b[2] = in.b[1];
    out.b[3] = in.b[0];
    return out.f;
}

double endian_swap_double(double v)
{
    union
    {
        double  d;
        uint8_t b[8];
    } in, out;

    in.d = v;
    out.b[0] = in.b[7];
    out.b[1] = in.b[6];
    out.b[2] = in.b[5];
    out.b[3] = in.b[4];
    out.b[4] = in.b[3];
    out.b[5] = in.b[2];
    out.b[6] = in.b[1];
    out.b[7] = in.b[0];
    return out.d;
}

uint32_t endian_parse_u32(const uint8_t* buf, uint8_t len, EndianType endian)
{
    if(len > 4) return 0;

    uint32_t v = 0;
    if (endian == ENDIAN_BIG)
    {
        for (uint8_t i = 0; i < len; i++)
            v = (v << 8) | buf[i];
    }
    else
    {
        for (uint8_t i = 0; i < len; i++)
            v |= ((uint32_t)buf[i]) << (8 * i);
    }
    return v;
}

void endian_write_u32(uint8_t* dst, uint8_t len, EndianType endian, uint32_t v)
{
    if(len > 4) return;

    if (endian == ENDIAN_BIG)
    {
        for (uint8_t i = 0; i < len; i++)
        {
            dst[i] = (uint8_t)((v >> (8 * (len - 1 - i))) & 0xFFu);
        }
    }
    else
    {
        for (uint8_t i = 0; i < len; i++)
        {
            dst[i] = (uint8_t)((v >> (8 * i)) & 0xFFu);
        }
    }
}

void endian_write_bytes(uint8_t* dst, const uint8_t* src, uint8_t len, EndianType endian)
{
    if (endian == ENDIAN_BIG && len > 1)
    {
        for (uint8_t i = 0; i < len; i++)
            dst[i] = src[len - 1 - i];
    }
    else
    {
        for (uint8_t i = 0; i < len; i++)
            dst[i] = src[i];
    }
}
