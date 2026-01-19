#include "utils.hpp"

//创建一个串口协议
AsciiProtocol default_proto((ProtocolFormat){"$$",2,2,4,65535,CheckType::CRC16,4,"\r\n",2});

AsciiProtocol::AsciiProtocol(const ProtocolFormat& fmt)
    : fmt_(fmt)
{
    reset();
}

void AsciiProtocol::reset()
{
    state_    = State::FIND_HEADER;
    index_    = 0;
    data_len_ = 0;
}

bool AsciiProtocol::input(char ch)
{
    switch (state_)
    {
    case State::FIND_HEADER:
        header_buf_[index_++] = ch;
        if (index_ == fmt_.header_len)
        {
            if (memcmp(header_buf_, fmt_.header, fmt_.header_len) == 0)
            {
                state_ = State::READ_CMD;
            }
            index_ = 0;
        }
        break;

    case State::READ_CMD:
        cmd_[index_++] = ch;
        if (index_ == fmt_.cmd_len)
        {
            cmd_[index_] = '\0';
            index_ = 0;
            state_ = State::READ_LEN;
        }
        break;

    case State::READ_LEN:
        len_buf_[index_++] = ch;
        if (index_ == fmt_.len_len)
        {
            data_len_ = asciiToUint(len_buf_, fmt_.len_len);
            if (data_len_ > fmt_.max_data_len)
            {
                reset();
                break;
            }
            index_ = 0;
            state_ = State::READ_DATA;
        }
        break;

    case State::READ_DATA:
        data_[index_++] = ch;
        if (index_ == data_len_)
        {
            data_[index_] = '\0';
            index_ = 0;
            state_ = State::READ_CHECK;
        }
        break;

    case State::READ_CHECK:
        check_buf_[index_++] = ch;
        if (index_ == fmt_.check_len)
        {
            uint16_t recv = asciiToUint(check_buf_, fmt_.check_len);
            uint16_t calc = calcCheck();
            if (recv != calc)
            {
                reset();
                break;
            }
            index_ = 0;
            state_ = State::READ_TAIL;
        }
        break;

    case State::READ_TAIL:
        tail_buf_[index_++] = ch;
        if (index_ == fmt_.tail_len)
        {
            if (memcmp(tail_buf_, fmt_.tail, fmt_.tail_len) == 0)
            {
                reset();
                return true;
            }
            reset();
        }
        break;
    }
    return false;
}

uint16_t AsciiProtocol::asciiToUint(const char* buf, uint8_t len) const
{
    uint16_t v = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        char c = buf[i];
        if (c >= '0' && c <= '9')
            v = v * 10 + (c - '0');
        else if (c >= 'A' && c <= 'F')
            v = (v << 4) + (c - 'A' + 10);
    }
    return v;
}

uint16_t AsciiProtocol::buildFrame(const char* cmd,
                                   const char* data,
                                   uint16_t    data_len,
                                   char*       out_buf,
                                   uint16_t    out_buf_size) const
{
    if (data_len > fmt_.max_data_len)
        return 0;

    uint16_t total_len =
        fmt_.header_len +
        fmt_.cmd_len +
        fmt_.len_len +
        data_len +
        fmt_.check_len +
        fmt_.tail_len;

    if (total_len > out_buf_size)
        return 0;

    uint16_t pos = 0;

    /* header */
    memcpy(&out_buf[pos], fmt_.header, fmt_.header_len);
    pos += fmt_.header_len;

    /* cmd */
    memcpy(&out_buf[pos], cmd, fmt_.cmd_len);
    pos += fmt_.cmd_len;

    /* length (ASCII) */
    uintToAscii(data_len, &out_buf[pos], fmt_.len_len);
    pos += fmt_.len_len;

    /* data */
    memcpy(&out_buf[pos], data, data_len);
    pos += data_len;

    /* check */
    uint16_t check = calcCheck(data, data_len);
    uintToAscii(check, &out_buf[pos], fmt_.check_len);
    pos += fmt_.check_len;

    /* tail */
    memcpy(&out_buf[pos], fmt_.tail, fmt_.tail_len);
    pos += fmt_.tail_len;

    return pos;
}

uint16_t AsciiProtocol::calcCheck(const char* data, uint16_t len) const
{
    if (fmt_.check_type == CheckType::SUM)
    {
        uint16_t sum = 0;
        for (uint16_t i = 0; i < len; i++)
            sum += data[i];
        return sum & 0xFF;
    }
    else if (fmt_.check_type == CheckType::CRC16)
    {
        uint16_t crc = 0xFFFF;
        for (uint16_t i = 0; i < len; i++)
        {
            crc ^= data[i];
            for (uint8_t j = 0; j < 8; j++)
                crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
        }
        return crc;
    }
    return 0;
}

uint16_t AsciiProtocol::calcCheck() const
{
    if (fmt_.check_type == CheckType::SUM)
    {
        uint16_t sum = 0;
        for (uint16_t i = 0; i < data_len_; i++)
            sum += data_[i];
        return sum & 0xFF;
    }
    else if (fmt_.check_type == CheckType::CRC16)
    {
        uint16_t crc = 0xFFFF;
        for (uint16_t i = 0; i < data_len_; i++)
        {
            crc ^= data_[i];
            for (uint8_t j = 0; j < 8; j++)
                crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
        }
        return crc;
    }
    return 0;
}

void AsciiProtocol::uintToAscii(uint16_t value, char* buf, uint8_t len) const
{
    for (int8_t i = len - 1; i >= 0; i--)
    {
        buf[i] = (value % 10) + '0';
        value /= 10;
    }
}
