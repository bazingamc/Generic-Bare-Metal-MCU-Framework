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

// 自定义发送函数
typedef void (*LogSendFunc)(void* ctx, const char* data, size_t len);

// 时间戳回调函数
typedef void (*LogTimeFunc)(char* buf, size_t len);

class Logger {
public:
    // 注册通道
    static void RegisterChannel(uint32_t channelMask, void* obj);
    static void RegisterChannel(uint32_t channelMask, LogSendFunc func, void* ctx = nullptr);

    // 日志级别控制
    static void EnableLevel(LogLevel level, bool enable);
    static void SetLevelMask(uint32_t mask);

    // 时间戳回调
    static void SetTimeCallback(LogTimeFunc func);

    // 日志输出
    static void Log(LogLevel level, uint32_t channelMask, const char* fmt, ...);
    static void VLog(LogLevel level, uint32_t channelMask, const char* fmt, va_list args);

private:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
