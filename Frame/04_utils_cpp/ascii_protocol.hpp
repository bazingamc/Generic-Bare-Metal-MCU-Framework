#pragma once


enum class CheckType : uint8_t
{
    NONE = 0,
    SUM,
    CRC16
};

struct ProtocolFormat
{
    const char* header;
    uint8_t     header_len;

    uint8_t     cmd_len;
    uint8_t     len_len;

    uint16_t    max_data_len;

    CheckType   check_type;
    uint8_t     check_len;

    const char* tail;
    uint8_t     tail_len;
};

class AsciiProtocol
{
public:
    explicit AsciiProtocol(const ProtocolFormat& fmt);

    void reset();

    // 输入一个 ASCII 字符，解析成功返回 true
    bool input(char ch);

    const char* cmd()  const { return cmd_; }
    const char* data() const { return data_; }
    uint16_t    dataLen() const { return data_len_; }

protected:
    uint16_t asciiToUint(const char* buf, uint8_t len) const;
    uint16_t calcCheck() const;

private:
    enum class State : uint8_t
    {
        FIND_HEADER,
        READ_CMD,
        READ_LEN,
        READ_DATA,
        READ_CHECK,
        READ_TAIL
    };

private:
    const ProtocolFormat& fmt_;
    State   state_;
    uint8_t index_;

    char header_buf_[16];
    char cmd_[16];
    char len_buf_[8];
    char data_[256];
    char check_buf_[8];
    char tail_buf_[8];

    uint16_t data_len_;
};
