#pragma once

/**
 * @brief 系统管理类
 * 提供系统级功能，如系统时间、延时等
 */
class System
{
    public:
        /**
         * @brief 运行系统任务
         * 执行所有设备的任务调度
         */
        static void run();

        /**
         * @brief 初始化系统
         * 初始化DWT和系统滴答定时器
         */
        static void init();
        
        /**
         * @brief 获取系统时间
         * @return uint64_t 当前系统时间（毫秒）
         */
        static uint64_t getSysTime();
        
        /**
         * @brief 毫秒级延时
         * @param ms 延时时间（毫秒）
         */
        static void delayMs(uint32_t ms);
        
        /**
         * @brief 微秒级延时
         * @param us 延时时间（微秒）
         */
        static void delayUs(uint32_t us);

        /**
         * @brief 时间标记类
         * 用于测量代码执行时间
         */
        class TimeMark
        {
            public:
                /**
                 * @brief 构造函数
                 * 初始化时间标记
                 */
                TimeMark();
                
                /**
                 * @brief 析构函数
                 */
                ~TimeMark();

                /**
                 * @brief 插入时间标记
                 * 更新开始时间
                 */
                void insert();
                
                /**
                 * @brief 获取经过的时间
                 * @return uint32_t 经过的时间（微秒）
                 */
                uint32_t get();

            private:
                uint32_t timeMarkStart;//时间标记开始
        };

    private:
        System() = delete;      // 禁止构造
        ~System() = delete;     // 禁止析构

        static uint64_t sysTime;//系统时间
        static void SystickIsrCallback();//系统定时器回调函数

    
};