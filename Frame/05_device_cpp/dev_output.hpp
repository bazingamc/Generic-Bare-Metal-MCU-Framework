#pragma once

/**
 * @brief 输出设备状态枚举
 */
typedef enum {
    OutState_Close = 0,//关闭
    OutState_Open,//打开
}OutState;

/**
 * @brief 输出设备参数结构体
 */
typedef struct 
{
    const char* name = "Output dev";//设备名称
    GpioIndex pin = PIN_END;//控制引脚
    GpioLevel validLevel = GPIO_LEVEL_LOW;//有效电平（设备打开时引脚输出的电平）
}OutputInitParam;


/**
 * @brief 输出设备类
 * 用于管理GPIO输出设备
 */
class Output
{
public:
    /**
     * @brief 构造函数
     */
    Output();
    
    /**
     * @brief 析构函数
     */
    ~Output();

    /**
     * @brief 初始化输出设备
     * @param param 输出设备参数
     */
    void init(OutputInitParam param);
    
    /**
     * @brief 打开输出设备
     */
    void open(); 
    
    /**
     * @brief 关闭输出设备
     */
    void close(); 
    
    /**
     * @brief 启动脉冲输出
     * @param cycle 周期，默认1000ms
     * @param duty 占空比，默认50%
     */
    void pulseOutputStart(u32 cycle = 1000 , float duty = 50); 
    
    /**
     * @brief 停止脉冲输出
     */
    void pulseOutputStop(); 
    
    /**
     * @brief 获取设备名称
     * @return const char* 设备名称
     */
    const char* getName() const;
    
    /**
     * @brief 获取当前状态
     * @return OutState 当前状态
     */
    OutState getState() const;
    
    /**
     * @brief 获取当前占空比
     * @return float 占空比数值
     */
    float getDuty() const;


    /**********************************************************
    * 静态方法
    **********************************************************/

    static void outputTask();//System::run()中调用
    static int getObjectCount();//System::run()中调用

private:
    bool isInit;//初始化标志

    //用于遍历所有对象
	static const int MAX_OBJECTS = 50;   // 最大对象数量
	static Output* objects[MAX_OBJECTS]; // 静态数组保存对象指针
	static int objectCount;               // 当前对象数量

    OutputInitParam initParam;//初始化参数
    float duty;//占空比 0 -100
    u32 cycle;//周期ms
    OutState state;

};