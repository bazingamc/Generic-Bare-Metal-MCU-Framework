#include "dev.hpp"


// 初始化系统时间静态变量
uint64_t System::sysTime = 0;

/**
 * @brief 系统滴答定时器中断回调函数
 * 递增系统时间计数器
 */
void System::SystickIsrCallback()
{
    System::sysTime++;
}

/**
 * @brief 运行系统任务
 * 执行输出和输入设备的任务调度
 */
void System::run()
{
    Output::outputTask();
    Input::inputTask();
}

/**
 * @brief 初始化系统
 * 初始化DWT和系统滴答定时器
 */
void System::init()
{
    hal_dwt.init();
    hal_systick.init(1000, System::SystickIsrCallback);
}

/**
 * @brief 获取系统时间
 * @return uint64_t 当前系统时间（毫秒）
 */
uint64_t System::getSysTime()
{
    return System::sysTime;
}

/**
 * @brief 毫秒级延时
 * @param ms 延时时间（毫秒）
 */
void System::delayMs(uint32_t ms)
{
    hal_dwt.delay(ms * 1000);
}

/**
 * @brief 微秒级延时
 * @param us 延时时间（微秒）
 */
void System::delayUs(uint32_t us)
{
    hal_dwt.delay(us);
}




/* TimeMark */

/**
 * @brief TimeMark构造函数
 * 记录当前CPU周期数作为起始时间
 */
System::TimeMark::TimeMark()
{
    this->timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief TimeMark析构函数
 */
System::TimeMark::~TimeMark()
{

}

/**
 * @brief 插入时间标记
 * 更新开始时间点
 */
void System::TimeMark::insert()
{
    System::TimeMark::timeMarkStart = hal_dwt.getCYCCNT();
}

/**
 * @brief 获取经过的时间
 * @return uint32_t 从起始时间到现在的经过时间（微秒）
 */
uint32_t System::TimeMark::get()
{
    return (hal_dwt.getCYCCNT() - System::TimeMark::timeMarkStart) / SYSTEM_CLOCK_MHZ;
}