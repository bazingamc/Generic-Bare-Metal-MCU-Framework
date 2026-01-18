#pragma once
#include "ascii_protocol.hpp"

typedef struct 
{
    const char* name = "Uart dev";//设备名称
    GpioIndex rxPin = PIN_END;//使用hal层默认引脚映射
    GpioIndex txPin = PIN_END;//使用hal层默认引脚映射
    UartIndex uart = _UART1;
    uint32_t baudrate = 115200; // 波特率
    void (*msgHandler)(void) = nullptr; // 消息处理回调函数
}UartInitParam;

class Uart
{
public:
    Uart(size_t rxBufSize, size_t txBufSize, AsciiProtocol* protocols[], uint8_t proto_count);

    void Init(UartInitParam param);

    

    /**********************************************************
    * 静态方法
    **********************************************************/

    static void uartTask();//System::run()中调用
    static int getObjectCount();//System::run()中调用


private:

    //用于遍历所有对象
	static const int MAX_OBJECTS = 10;   // 最大对象数量
	static Uart* objects[MAX_OBJECTS]; // 静态数组保存对象指针
	static int objectCount;               // 当前对象数量

    UartInitParam initParam;//初始化参数

    RingBuffer<uint8_t> rxBuffer_;
    RingBuffer<uint8_t> txBuffer_;

    AsciiProtocol** protocols_;
    uint8_t proto_count_;

    void (*msgHandler)(void);

    // ISR 中调用
    void onRxChar(uint8_t ch);
};
