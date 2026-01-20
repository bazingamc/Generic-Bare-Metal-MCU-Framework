#pragma once

enum class TimeFormat {
    HH_MM_SS_MS,        // 12:34:56.789
    YYYY_MM_DD_HH_MM_SS,// 2026-01-19 12:34:56
    MS_COUNT,           // Millisecond count
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
 * @brief System management class
 * Provides system-level functions, such as system time, delay, etc.
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

        //Get time
        static uint64_t getSysTime();
        static char* getSysTime(TimeFormat fmt);
        static TimeInfo getSysDateTime();

        //Blocking delay
        static void delayMs(uint32_t ms);
        static void delayUs(uint32_t us);

        /**
         * @brief Time marker class
         * Used to measure code execution time
         */
        class TimeMark
        {
        public:
            
            TimeMark();
            ~TimeMark();
            void insert();
            uint32_t get();
        private:
            uint32_t timeMarkStart;//Time marker start
        };

        private:
        Time() = delete;      // Prevent construction
        ~Time() = delete;     // Prevent destruction
        static uint64_t sysTime;//System time
        static TimeInfo timeInfo;//Time info structure
        
        // Static buffers
        static char s_bufHHMMSS[16];
        static char s_bufYYYYMMDD[32];
        static char s_bufMS[24];
        static char s_bufHHMMSSMS[16];
        static char s_bufYYYYMMDDHHMMSS[32];

        static void SystickIsrCallback();//System timer callback function
    };
    
private:
    System() = delete;      // Prevent construction
    ~System() = delete;     // Prevent destruction


};