#pragma once
enum class CheckType : uint8_t
{
    NONE = 0,
    SUM,
    CRC16
};

struct ProtocolFormat
{
    const uint8_t*    header;
    uint8_t     header_len;

    uint8_t     cmd_len;
    uint8_t     len_len;

    CheckType   check_type;
    uint8_t     check_len;

    const uint8_t*    tail;
    uint8_t     tail_len;
};

class Protocol
{
public:
    Protocol(const ProtocolFormat& fmt);

    /* ---------- Receive Parsing ---------- */
    void reset();
    bool input(char ch);

    const uint8_t* cmd()  const { return cmd_; }
    const uint8_t* data() const { return data_; }
    uint16_t    dataLen() const { return data_len_; }


    /* ---------- Send Packing ---------- */
    // Returns the length of the generated frame, 0 indicates failure
    uint16_t buildFrame(uint8_t* cmd,
                        uint8_t* data,
                        uint16_t    data_len,
                        uint8_t*       out_buf,
                        uint16_t    out_buf_size) const;

protected:
    uint16_t calcCheck() const;
    uint16_t calcCheck(uint8_t* data, uint16_t len) const;

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

    uint8_t header_buf_[16];
    uint8_t cmd_[16];
    uint8_t len_buf_[8];
    uint8_t data_[1024];
    uint8_t check_buf_[8];
    uint8_t tail_buf_[8];

    uint32_t data_len_;

    
};

extern Protocol default_proto;