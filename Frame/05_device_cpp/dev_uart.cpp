#include "dev.hpp"

// Static member initialization
Uart* Uart::objects[Uart::MAX_OBJECTS] = { nullptr };
int Uart::objectCount = 0;
Uart* Uart::instance_ = nullptr;
Uart::Uart(size_t rxBufSize, size_t txBufSize)
    : rxBuffer_(rxBufSize),
      txBuffer_(txBufSize)
{
    if (objectCount < MAX_OBJECTS) 
	{
		objects[objectCount++] = this;
	}
	else 
	{
		// If capacity exceeded, print warning or handle
	}
}

Uart::~Uart()
{
    for (int i = 0; i < objectCount; i++) 
	{
		if (objects[i] == this) 
		{
			// Replace the current position with the last object
			objects[i] = objects[objectCount - 1];
			objects[objectCount - 1] = nullptr;
			objectCount--;
			break;
		}
	}
}

void Uart::Init(UartInitParam param)
{
    memcpy(&this->initParam, &param, sizeof(param));
    hal_uart.init(param.uart, param.baudrate, param.txPin, param.rxPin);
    instance_ = this;
    hal_uart.register_rx_callback(this->initParam.uart, &Uart::rxCallbackThunk);
    this->msgHandler = param.msgHandler;
    this->protocols_ = param.protocols;
    this->proto_count_ = param.proto_count;
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

        if(System::Time::getSysTime() - obj->received_time_ > 10)
        {
            obj->protocols_[i]->reset();
        }
		if(obj->rxBuffer_.pop(ch))
        {
            obj->received_time_ = System::Time::getSysTime();
            if (obj->protocols_[i]->input(ch))
            {
                if(obj->msgHandler != nullptr)obj->msgHandler(obj, obj->protocols_[i]);
            }
        }
	}

}

void Uart::Send(u16 len, uint8_t* data)
{
    hal_uart.send_bytes(this->initParam.uart, len, data);
    // hal_uart.send_bytes_dma(this->initParam.uart, data, len);
}

void Uart::Send(Protocol *protocol, uint32_t cmd, uint32_t len, uint8_t* data)
{
    uint8_t frame[512];
    uint16_t frame_len = protocol->buildFrame((uint8_t*)&cmd, (uint8_t*)data, len, frame, sizeof(frame));
    if (frame_len > 0)
    {
        this->Send(frame_len, (uint8_t*)frame);
    }
}

void Uart::rxCallbackThunk(uint8_t ch)
{
    if (instance_)
    {
        instance_->onRxChar(ch);
    }
}