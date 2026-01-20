#include "dev.hpp"

// Static member initialization
Uart* Uart::objects[Uart::MAX_OBJECTS] = { nullptr };
int Uart::objectCount = 0;
Uart::Uart(size_t rxBufSize, size_t txBufSize, AsciiProtocol* protocols[], uint8_t proto_count)
    : protocols_(protocols),
      proto_count_(proto_count),
      rxBuffer_(rxBufSize),
      txBuffer_(txBufSize)
{

}

void Uart::Init(UartInitParam param)
{
    memcpy(&this->initParam, &param, sizeof(param));
    hal_uart.init(param.uart, param.baudrate, param.txPin, param.rxPin);
    this->initParam.msgHandler = param.msgHandler;
}

int Uart::getObjectCount()
{
    return Uart::objectCount;
}
void Uart::onRxChar(uint8_t ch)
{
    this->rxBuffer_.push(ch);
}

void Uart::uartTask()
{
    uint8_t ch;

    for (uint16_t i = 0; i < Uart::objectCount; i++)
	{
		Uart* obj = Uart::objects[i];
		if(obj->rxBuffer_.pop(ch))
        {
            if (obj->protocols_[i]->input(ch))
            {
                if(obj->msgHandler != nullptr)obj->msgHandler(obj);
            }
        }
	}

}

void Uart::Send(u16 len, uint8_t* data)
{
    hal_uart.send_bytes(this->initParam.uart, len, data);
}