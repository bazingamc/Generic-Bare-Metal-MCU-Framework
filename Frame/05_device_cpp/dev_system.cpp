#include "dev.hpp";


// Initialize system time static variables
uint64_t System::Time::sysTime = 0;
char System::Time::s_bufHHMMSS[16] = {0};
char System::Time::s_bufYYYYMMDD[32] = {0};
char System::Time::s_bufMS[24] = {0};
char System::Time::s_bufHHMMSSMS[16] = {0};
char System::Time::s_bufYYYYMMDDHHMMSS[32] = {0};
TimeInfo System::Time::timeInfo = {}; // Define timeInfo static member variable

/**
 * @brief System tick timer interrupt callback function
 * Increment system time counter
 */
void System::Time::SystickIsrCallback()
{
    sysTime++;
}



void System::Time::init()
{
    hal_dwt.init();
    hal_systick.init(1000, System::Time::SystickIsrCallback);
}

/**
 * @brief Get system time
 * @return uint64_t Current system time (milliseconds)
 */
uint64_t System::Time::getSysTime()
{
    return sysTime;
}

TimeInfo System::Time::getSysDateTime()
{
    uint64_t ms = sysTime;

    timeInfo.hour = (ms / 3600000) % 24;
    timeInfo.minute = (ms / 60000) % 60;
    timeInfo.second = (ms / 1000) % 60;
    timeInfo.millisecond = ms % 1000;

    return timeInfo;
}

char* System::Time::getSysTime(TimeFormat fmt)
{
    uint64_t ms = sysTime;

    uint32_t h = (ms / 3600000) % 24;
    uint32_t m = (ms / 60000) % 60;
    uint32_t s = (ms / 1000) % 60;
    uint32_t t = ms % 1000;

    switch(fmt) {
        case TimeFormat::HH_MM_SS_MS:
            snprintf(s_bufHHMMSS, sizeof(s_bufHHMMSS), "%02u:%02u:%02u.%03u", h, m, s, t);
            return s_bufHHMMSS;

        case TimeFormat::YYYY_MM_DD_HH_MM_SS:
            snprintf(s_bufYYYYMMDD, sizeof(s_bufYYYYMMDD), "%04u-%02u-%02u %02u:%02u:%02u",timeInfo.year, timeInfo.month, timeInfo.day, h, m, s);
            return s_bufYYYYMMDD;

        case TimeFormat::MS_COUNT:
            snprintf(s_bufMS, sizeof(s_bufMS), "%llu", ms);
            return s_bufMS;

        case TimeFormat::HHMMSSMS:
            snprintf(s_bufHHMMSSMS, sizeof(s_bufHHMMSSMS), "%02u%02u%02u%03u", h, m, s, t);
            return s_bufHHMMSSMS;

        case TimeFormat::YYYYMMDDHHMMSS:
            snprintf(s_bufYYYYMMDDHHMMSS, sizeof(s_bufYYYYMMDDHHMMSS),
                     "%04u%02u%02u%02u%02u%02u",timeInfo.year, timeInfo.month, timeInfo.day, h, m, s);
            return s_bufYYYYMMDDHHMMSS;

        default:
            return "";
    }
}

/**
 * @brief Millisecond delay
 * @param ms Delay time (milliseconds)
 */
void System::Time::delayMs(uint32_t ms)
{
    hal_dwt.delay(ms * 1000);
}

/**
 * @brief Microsecond delay
 * @param us Delay time (microseconds)
 */
void System::Time::delayUs(uint32_t us)
{
    hal_dwt.delay(us);
}




/* TimeMark */

/**
 * @brief TimeMark constructor
 * Record current CPU cycle count as start time
 */
System::Time::TimeMark::TimeMark()
{
    this->timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief TimeMark destructor
 */
System::Time::TimeMark::~TimeMark()
{

}

/**
 * @brief Insert time marker
 * Update start time point
 */
void System::Time::TimeMark::insert()
{
    timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief Get elapsed time
 * @return uint32_t Elapsed time from start time to now (microseconds)
 */
uint32_t System::Time::TimeMark::get()
{
    return (hal_dwt.getCYCCNT() - timeMarkStart) / SYSTEM_CLOCK_MHZ;
}



/**
 * @brief Initialize system
 * Initialize DWT and system tick timer
 */
void System::init()
{
    SysClockConfig clk_cfg =
    {
        .clk_src = SYSCLK_SRC_HSE,
        .hse_freq = 25000000,
        .sysclk_freq = 180000000
    };

    SystemClock_Init(&clk_cfg);

    System::Time::init();
}

/**
 * @brief Run system tasks
 * Execute output and input device task scheduling
 */
void System::run()
{
    if(Output::getObjectCount())
        Output::outputTask();

    if(Input::getObjectCount())
        Input::inputTask();

    if(Uart::getObjectCount())
        Uart::uartTask();

    if(Task::getObjectCount())
        Task::run(System::Time::getSysTime());
}