#pragma once
#include "ascii_protocol.hpp"

class Uart;

typedef struct 
{
    const char* name = "Uart dev";//Device name
    GpioIndex rxPin = PIN_END;//Use hal layer default pin mapping
    GpioIndex txPin = PIN_END;//Use hal layer default pin mapping
    UartIndex uart = _UART1;
    uint32_t baudrate = 115200; // Baud rate
    void (*msgHandler)(Uart*) = nullptr; // Message handling callback function
}UartInitParam;

class Uart
{
public:
    Uart(size_t rxBufSize, size_t txBufSize, AsciiProtocol* protocols[], uint8_t proto_count);

    void Init(UartInitParam param);
    void Send(u16 len, uint8_t* data);

    

    /**********************************************************
    * Static methods
    **********************************************************/

    static void uartTask();//Called in System::run()
    static int getObjectCount();//Called in System::run()


private:

    //Used to traverse all objects
	static const int MAX_OBJECTS = 10;   // Maximum object count
	static Uart* objects[MAX_OBJECTS]; // Static array to store object pointers
	static int objectCount;               // Current object count

    UartInitParam initParam;//Initialization parameters

    RingBuffer<uint8_t> rxBuffer_;
    RingBuffer<uint8_t> txBuffer_;

    AsciiProtocol** protocols_;
    uint8_t proto_count_;

    void (*msgHandler)(Uart*);

    // Called from ISR
    void onRxChar(uint8_t ch);
};