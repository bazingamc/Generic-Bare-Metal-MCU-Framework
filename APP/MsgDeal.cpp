#include "APP.hpp"

void MsgDeal(Uart* obj, Protocol* protocol)
{
    uint32_t cmd = protocol->cmd();
    uint16_t dataLen = protocol->dataLen();
    const uint8_t* data = protocol->data();

    switch (cmd)
    {
    case 0x01:
    {
        LOG_INFO(LOG_CH_UART, "hello\r\n");
    }
    break;
    case 0x02:
    {
        if(data[0])
        {
            ledR.pulseOutputStart();
        }
        else
        {
            ledR.pulseOutputStop();
        }
        if(data[1])
        {
            ledG.pulseOutputStart();
        }
        else
        {
            ledG.pulseOutputStop();
        }
        if(data[2])
        {
            ledB.pulseOutputStart();
        }
        else
        {
            ledB.pulseOutputStop();
        }
    }
    break;
    case 0x03:
    {
        if(data[0])
        {
            beep.pulseOutputStart(3000, 10);
        }
        else
        {
            beep.pulseOutputStop();
        }
    }
    break;

    case 0x04:
    {
        uint8_t response_data[4] = {1,2,3,4};
        uart1.Send(&myproto, cmd, 4, (uint8_t*)response_data);
    }
    break;

    case 0x05:
    {
        if(data[0])
        {
            t1.start();
        }
        else    
        { 
            t1.stop();
        }
    }
    break;

    
    default:
        break;
    }
}