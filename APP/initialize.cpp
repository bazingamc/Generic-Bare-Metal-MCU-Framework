#include "APP.hpp"

Output ledR, ledG, ledB;
Output beep;
Uart uart1(1024, 1024);
void APP_Init() 
{
    System::init();

	ledR.init((OutputInitParam){"ledR", PH10, GPIO_LEVEL_LOW});
	ledG.init((OutputInitParam){"ledG", PH11, GPIO_LEVEL_LOW});
	ledB.init((OutputInitParam){"ledB", PH12, GPIO_LEVEL_LOW});
	ledG.pulseOutputStart();

	beep.init((OutputInitParam){"BEEP", PI11, GPIO_LEVEL_HIGH});

	
	static Protocol* protos[] = {&default_proto};
	uart1.Init((UartInitParam){"UART1", PA10, PA9, _UART1, 115200, protos, 1, MsgDeal});

	// log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	Logger::SetTimeCallback(System::Time::getSysTime);
	LOG_INFO(LOG_CH_UART, "hello world !\r\n");

	beep.open();
	System::Time::delayMs(100);
	beep.close();

}