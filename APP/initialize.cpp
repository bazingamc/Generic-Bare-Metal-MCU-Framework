#include "APP.hpp"

Output led1;
// Protocol list
AsciiProtocol* protos[] = {&default_proto};
Uart uart1(1024, 1024, protos, 1);
void APP_Init() 
{
    System::init();

    // Program running indicator light
	OutputInitParam param;
	param.name = "LED1";
	param.pin = PH10;
	param.validLevel = GPIO_LEVEL_LOW;
	led1.init(param);
	led1.pulseOutputStart();// Start pulse output, default parameters: period 1000ms, duty cycle 50%

	// UART
	UartInitParam uartParam;
	uartParam.name = "UART1";
	uartParam.uart = _UART1;
	uartParam.baudrate = 115200;
	uart1.Init(uartParam);

	// log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	Logger::SetTimeCallback(System::Time::getSysTime);
	LOG_INFO(LOG_CH_UART, "hello world !\r\n");
}