#include "APP.hpp"

Output led1;
Output beep;
// Protocol list
AsciiProtocol* protos[] = {&default_proto};
Uart uart1(1024, 1024, protos, 1);
void APP_Init() 
{
    System::init();

	led1.init((OutputInitParam){"LED1", PH11, GPIO_LEVEL_LOW});
	led1.pulseOutputStart();// Start pulse output, default parameters: period 1000ms, duty cycle 50%

	beep.init((OutputInitParam){"BEEP", PI11, GPIO_LEVEL_HIGH});

	uart1.Init((UartInitParam){"UART1", PIN_END, PIN_END, _UART1, 115200, nullptr});

	// log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	Logger::SetTimeCallback(System::Time::getSysTime);
	LOG_INFO(LOG_CH_UART, "hello world !\r\n");

	beep.open();
	System::Time::delayMs(100);
	beep.close();

}