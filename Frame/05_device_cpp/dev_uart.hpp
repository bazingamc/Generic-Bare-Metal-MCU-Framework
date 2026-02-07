#pragma once
#include "protocol.hpp"

class Uart;


typedef struct 
{
    const char* name = "Uart dev";//Device name
    GpioIndex rxPin = PIN_END;//Use hal layer default pin mapping
    GpioIndex txPin = PIN_END;//Use hal layer default pin mapping
    UartIndex uart = _UART1;
    uint32_t baudrate = 115200; // Baud rate
    Protocol** protocols;
    uint8_t proto_count;
    void (*msgHandler)(Uart*, Protocol*) = nullptr; // Message handling callback function
}UartInitParam;

class Uart
{
public:
    Uart(size_t rxBufSize, size_t txBufSize);
    ~Uart();

    void Init(UartInitParam param);
    void Send(u16 len, uint8_t* data);
    void Send(Protocol *protocol, uint32_t cmd, uint32_t len, uint8_t* data);

    

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

    Protocol** protocols_;
    uint8_t proto_count_;

    uint64_t received_time_;

    void (*msgHandler)(Uart*, Protocol*);

    // Called from ISR
    void onRxChar(uint8_t ch);
    static void rxCallbackThunk(uint8_t ch);
    static Uart* instance_;
};