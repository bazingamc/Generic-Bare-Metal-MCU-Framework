#pragma once

#define LOG_DEBUG(channel, ...) Logger::Log(LogLevel::DEBUG, channel, __VA_ARGS__)
#define LOG_INFO(channel, ...)  Logger::Log(LogLevel::INFO,  channel, __VA_ARGS__)
#define LOG_WARN(channel, ...)  Logger::Log(LogLevel::WARN,  channel, __VA_ARGS__)
#define LOG_ERROR(channel, ...) Logger::Log(LogLevel::ERROR, channel, __VA_ARGS__)

enum class LogLevel : uint8_t {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    COUNT
};

enum LogChannel : uint32_t {
    LOG_CH_UART = (1u << 0),
    LOG_CH_CAN  = (1u << 1),
    LOG_CH_USB  = (1u << 2),
};

// Custom send function
typedef void (*LogSendFunc)(void* ctx, const char* data, size_t len);

// Timestamp callback function
typedef char* (*LogTimeFunc)(TimeFormat fmt);

class Logger {
public:
    // Register channel
    static void RegisterChannel(uint32_t channelMask, void* obj);
    static void RegisterChannel(uint32_t channelMask, LogSendFunc func, void* ctx = nullptr);

    // Log level control
    static void EnableLevel(LogLevel level, bool enable);
    static void SetLevelMask(uint32_t mask);

    // Timestamp callback
    static void SetTimeCallback(LogTimeFunc func);

    // Log output
    static void Log(LogLevel level, uint32_t channelMask, const char* fmt, ...);
    static void VLog(LogLevel level, uint32_t channelMask, const char* fmt, va_list args);

private:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};