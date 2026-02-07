#include "utils.hpp"

const uint8_t default_header[] = {0xaa, 0x55};
const uint8_t default_tail[] = {0x0d, 0x0a};
const ProtocolFormat default_proto_fmt = {default_header,2,1,1,CheckType::NONE,1,default_tail,2};
Protocol default_proto(default_proto_fmt);
Protocol::Protocol(const ProtocolFormat& fmt)
    : fmt_(fmt)
{
    reset();
}

void Protocol::reset()
{
    state_    = State::FIND_HEADER;
    index_    = 0;
}

bool Protocol::input(char ch)
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
            index_ = 0;
            data_len_ = 0;
            state_ = State::READ_LEN;
        }
        break;

    case State::READ_LEN:
        len_buf_[index_++] = ch;
        if (index_ == fmt_.len_len)
        {
            switch (fmt_.len_len)
            {
            case 1: data_len_ = *((uint8_t*)len_buf_);break;
            case 2: data_len_ = *((uint16_t*)len_buf_);break;
            case 4: data_len_ = *((uint32_t*)len_buf_);break;
            default:data_len_ = *((uint8_t*)len_buf_);break;
            }
            index_ = 0;
            if(data_len_ == 0)
            {
                state_ = State::READ_CHECK;
            }
            else
            {
                state_ = State::READ_DATA;
            }
        }
        break;

    case State::READ_DATA:
        data_[index_++] = ch;
        if (index_ == data_len_)
        {
            index_ = 0;
            state_ = State::READ_CHECK;
        }
        break;

    case State::READ_CHECK:
        check_buf_[index_++] = ch;
        if (index_ == fmt_.check_len)
        {
            uint16_t recv = 0;
            for (uint8_t i = 0; i < fmt_.check_len; i++)
            {
               recv = recv << 8 | check_buf_[i];
            }
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



uint16_t Protocol::buildFrame(uint8_t* cmd,
                                   uint8_t* data,
                                   uint16_t    data_len,
                                   uint8_t*       out_buf,
                                   uint16_t    out_buf_size) const
{

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

    /* length */
    memcpy(&out_buf[pos], &data_len, fmt_.len_len);
    pos += fmt_.len_len;

    /* data */
    memcpy(&out_buf[pos], data, data_len);
    pos += data_len;

    /* check */
    uint16_t check = calcCheck(data, data_len);
    memcpy(&out_buf[pos], &check, fmt_.check_len);
    pos += fmt_.check_len;

    /* tail */
    memcpy(&out_buf[pos], fmt_.tail, fmt_.tail_len);
    pos += fmt_.tail_len;

    return pos;
}

uint16_t Protocol::calcCheck(uint8_t* data, uint16_t len) const
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

uint16_t Protocol::calcCheck() const
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

