#include "dev.hpp"


// 初始化系统时间静态变量
uint64_t System::Time::sysTime = 0;
char System::Time::s_bufHHMMSS[16] = {0};
char System::Time::s_bufYYYYMMDD[32] = {0};
char System::Time::s_bufMS[24] = {0};
char System::Time::s_bufHHMMSSMS[16] = {0};
char System::Time::s_bufYYYYMMDDHHMMSS[32] = {0};

/**
 * @brief 系统滴答定时器中断回调函数
 * 递增系统时间计数器
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
 * @brief 获取系统时间
 * @return uint64_t 当前系统时间（毫秒）
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

const char* System::Time::getSysTime(TimeFormat fmt)
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
 * @brief 毫秒级延时
 * @param ms 延时时间（毫秒）
 */
void System::Time::delayMs(uint32_t ms)
{
    hal_dwt.delay(ms * 1000);
}

/**
 * @brief 微秒级延时
 * @param us 延时时间（微秒）
 */
void System::Time::delayUs(uint32_t us)
{
    hal_dwt.delay(us);
}




/* TimeMark */

/**
 * @brief TimeMark构造函数
 * 记录当前CPU周期数作为起始时间
 */
System::Time::TimeMark::TimeMark()
{
    this->timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief TimeMark析构函数
 */
System::Time::TimeMark::~TimeMark()
{

}

/**
 * @brief 插入时间标记
 * 更新开始时间点
 */
void System::Time::TimeMark::insert()
{
    timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief 获取经过的时间
 * @return uint32_t 从起始时间到现在的经过时间（微秒）
 */
uint32_t System::Time::TimeMark::get()
{
    return (hal_dwt.getCYCCNT() - timeMarkStart) / SYSTEM_CLOCK_MHZ;
}



/**
 * @brief 初始化系统
 * 初始化DWT和系统滴答定时器
 */
void System::init()
{
    System::Time::init();
}

/**
 * @brief 运行系统任务
 * 执行输出和输入设备的任务调度
 */
void System::run()
{
    if(Output::getObjectCount())Output::outputTask();
    if(Input::getObjectCount())Input::inputTask();
    if(Uart::getObjectCount())Uart::uartTask();
    if(Task::getObjectCount())Task::run();
}