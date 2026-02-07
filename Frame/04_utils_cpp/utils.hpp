#pragma once

#include "chip.h"
#include "endian.h"
#include "ring_buffer.hpp"
#include "task.hpp"
#include "protocol.hpp"

inline void BytesToString(uint8_t* data, uint16_t len, char* outStr, size_t outStrSize)
{
    if (data == nullptr || len == 0 || outStr == nullptr || outStrSize == 0) {
        return ;
    }

    size_t pos = 0;
    for (uint16_t i = 0; i < len; i++) {
        int written = snprintf(&outStr[pos], outStrSize - pos, "%02X ", data[i]);
        if (written < 0 || (size_t)written >= outStrSize - pos) {
            break; // Output buffer full or error
        }
        pos += written;
    }
    return ;
}
