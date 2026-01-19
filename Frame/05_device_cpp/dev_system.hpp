#pragma once

enum class TimeFormat {
    HH_MM_SS_MS,        // 12:34:56.789
    YYYY_MM_DD_HH_MM_SS,// 2026-01-19 12:34:56
    MS_COUNT,           // 毫秒数
    HHMMSSMS,           // 123456789
    YYYYMMDDHHMMSS      // 20260119123456
};

struct TimeInfo
{
    uint16_t year = 2026;
    uint8_t month = 1;
    uint8_t day = 1;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    uint16_t millisecond = 0;
};

/**
 * @brief 系统管理类
 * 提供系统级功能，如系统时间、延时等
 */
class System
{
public:
    static void run();
    static void init();

    class Time
    {
    public:
        static void init();

        //获取时间
        static uint64_t getSysTime();
        static const char* getSysTime(TimeFormat fmt);
        static TimeInfo getSysDateTime();

        //阻塞延迟
        static void delayMs(uint32_t ms);
        static void delayUs(uint32_t us);

        /**
         * @brief 时间标记类
         * 用于测量代码执行时间
         */
        class TimeMark
        {
        public:
            
            TimeMark();
            ~TimeMark();
            void insert();
            uint32_t get();
        private:
            uint32_t timeMarkStart;//时间标记开始
        };

        private:
        Time() = delete;      // 禁止构造
        ~Time() = delete;     // 禁止析构
        static uint64_t sysTime;//系统时间
        static TimeInfo timeInfo;//时间信息结构体
        
        // 静态缓冲区
        static char s_bufHHMMSS[16];
        static char s_bufYYYYMMDD[32];
        static char s_bufMS[24];
        static char s_bufHHMMSSMS[16];
        static char s_bufYYYYMMDDHHMMSS[32];

        static void SystickIsrCallback();//系统定时器回调函数
    };
    
private:
    System() = delete;      // 禁止构造
    ~System() = delete;     // 禁止析构


};