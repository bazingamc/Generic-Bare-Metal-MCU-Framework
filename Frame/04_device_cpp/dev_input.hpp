#pragma once

/**
 * @brief 输入设备状态枚举
 */
typedef enum {
    InputState_Inactive = 0,//未激活
    InputState_Active,//激活
}InputState;

/**
 * @brief 输入设备参数结构体
 */
typedef struct 
{
    const char* name = "Input dev";//设备名称
    GpioIndex pin = PIN_END;//检测引脚
    GpioLevel activeLevel = GPIO_LEVEL_HIGH;//有效电平（检测到此电平时认为输入激活）
    uint8_t filterCount = 1;//滤波次数，默认为1（无滤波）
}InputParam;


/**
 * @brief 输入设备类
 * 用于管理GPIO输入设备，支持滤波功能
 */
class Input
{
public:
    /**
     * @brief 构造函数
     */
    Input();
    
    /**
     * @brief 析构函数
     */
    ~Input();

    /**
     * @brief 初始化输入设备
     * @param param 输入设备参数
     */
    void init(InputParam param);
    
    /**
     * @brief 检测输入是否处于激活状态
     * @return true 激活状态, false 非激活状态
     */
    bool isActive(); 
    
    /**
     * @brief 检测输入是否处于非激活状态
     * @return true 非激活状态, false 激活状态
     */
    bool isInactive(); 
    
    /**
     * @brief 获取当前状态
     * @return InputState 当前状态
     */
    InputState getState() const;
    
    /**
     * @brief 获取设备名称
     * @return const char* 设备名称
     */
    const char* getName() const;


    /**********************************************************
    * 静态方法
    **********************************************************/

    /**
     * @brief 输入设备任务处理函数
     * 需要定期调用以更新所有输入设备的状态
     */
    static void inputTask();//循环调用

private:
    bool isInit;//初始化标志

    //用于遍历所有对象
	static const int MAX_OBJECTS = 30;   // 最大对象数量
	static Input* objects[MAX_OBJECTS]; // 静态数组保存对象指针
	static int objectCount;             // 当前对象数量

    InputParam initParam;//初始化参数
    InputState state;      // 当前状态
    InputState lastState;  // 上次状态
    
    // 滤波相关变量
    uint8_t activeCounter;   // 激活状态计数器
    uint8_t inactiveCounter; // 非激活状态计数器
    bool currentReading;     // 当前读取的电平状态
};