#include "dev.hpp"
#define LOG_BUFFER_SIZE   128
#define LOG_HEADER_SIZE   32
#define MAX_LOG_CHANNELS  32

struct LogChannelDesc {
    void*       obj;
    LogSendFunc sendFunc;
};

static LogChannelDesc s_channels[MAX_LOG_CHANNELS] = {0};
static uint32_t s_levelMask =
    (1u << static_cast<uint8_t>(LogLevel::DEBUG)) |
    (1u << static_cast<uint8_t>(LogLevel::INFO))  |
    (1u << static_cast<uint8_t>(LogLevel::WARN))  |
    (1u << static_cast<uint8_t>(LogLevel::ERROR));

// Timestamp callback
static LogTimeFunc s_timeFunc = nullptr;

// Log level strings
static const char* LevelToStr(LogLevel lvl)
{
    switch(lvl) {
        case LogLevel::DEBUG: return "D";
        case LogLevel::INFO:  return "I";
        case LogLevel::WARN:  return "W";
        case LogLevel::ERROR: return "E";
        default:              return "?";
    }
}

// Register channel
void Logger::RegisterChannel(uint32_t mask, void* obj)
{
    for (uint8_t i = 0; i < MAX_LOG_CHANNELS; ++i) {
        if ((mask >> i) & 0x01) {
            s_channels[i].obj = obj;
            s_channels[i].sendFunc = nullptr;
        }
    }
}

void Logger::RegisterChannel(uint32_t mask, LogSendFunc func, void* ctx)
{
    for (uint8_t i = 0; i < MAX_LOG_CHANNELS; ++i) {
        if ((mask >> i) & 0x01) {
            s_channels[i].sendFunc = func;
            s_channels[i].obj = ctx;
        }
    }
}

// Log level control
void Logger::EnableLevel(LogLevel lvl, bool enable)
{
    uint32_t bit = 1u << static_cast<uint8_t>(lvl);
    if (enable) s_levelMask |= bit;
    else        s_levelMask &= ~bit;
}

void Logger::SetLevelMask(uint32_t mask)
{
    s_levelMask = mask;
}

// Set timestamp callback
void Logger::SetTimeCallback(LogTimeFunc func)
{
    s_timeFunc = func;
}

// Output log
void Logger::Log(LogLevel lvl, uint32_t mask, const char* fmt, ...)
{
    if ((s_levelMask & (1u << static_cast<uint8_t>(lvl))) == 0)
        return;

    va_list args;
    va_start(args, fmt);
    VLog(lvl, mask, fmt, args);
    va_end(args);
}

void Logger::VLog(LogLevel lvl, uint32_t mask, const char* fmt, va_list args)
{
    char body[LOG_BUFFER_SIZE];
    char header[LOG_HEADER_SIZE];
    char timeStr[30] = {0};

    int bodyLen = vsnprintf(body, sizeof(body), fmt, args);
    if (bodyLen <= 0) return;

    // Timestamp
    if (s_timeFunc) {
        strncpy(timeStr, s_timeFunc(TimeFormat::HH_MM_SS_MS), sizeof(timeStr) - 1); // e.g., "12:34:56.789"
        timeStr[sizeof(timeStr) - 1] = '\0'; // Ensure string ends with null terminator
    }

    int headerLen = snprintf(header, sizeof(header), "[%s][%s] ", timeStr, LevelToStr(lvl));

    char buf[LOG_HEADER_SIZE + LOG_BUFFER_SIZE];
    memcpy(buf, header, headerLen);
    memcpy(buf + headerLen, body, bodyLen);
    int totalLen = headerLen + bodyLen;

    for (uint8_t i = 0; i < MAX_LOG_CHANNELS; ++i) {
        if ((mask >> i) & 0x01) {
            auto& ch = s_channels[i];
            if (ch.obj && ch.sendFunc == nullptr) {
                switch(i) {
                    case 0: static_cast<class Uart*>(ch.obj)->Send(totalLen, (uint8_t*)buf); break;
                    // case 1: static_cast<class Can*>(ch.obj)->Send(buf, totalLen); break;
                    // case 2: static_cast<class Usb*>(ch.obj)->Send(buf, totalLen); break;
                    default: if(ch.sendFunc) ch.sendFunc(ch.obj, buf, totalLen); break;
                }
            } else if(ch.sendFunc) {
                ch.sendFunc(ch.obj, buf, totalLen);
            }
        }
    }
}