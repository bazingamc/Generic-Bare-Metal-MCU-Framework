#include "APP.hpp"

Output ledR, ledG, ledB;
Output beep;
Uart uart1(1024, 1024);

//创建自定义协议
const uint8_t myproto_header[] = {0x88};
const uint8_t myproto_tail[] = {0x0d, 0x0a};
const ProtocolFormat myproto_fmt = {myproto_header,1,2,2,CheckType::NONE,1,myproto_tail,2,EndianType::ENDIAN_LITTLE};
Protocol myproto(myproto_fmt);
//协议列表
Protocol* protos[] = {&default_proto, &myproto};

void APP_Init() 
{
    System::init();

	ledR.init((OutputInitParam){"ledR", PH10, GPIO_LEVEL_LOW});
	ledG.init((OutputInitParam){"ledG", PH11, GPIO_LEVEL_LOW});
	ledB.init((OutputInitParam){"ledB", PH12, GPIO_LEVEL_LOW});
	ledG.pulseOutputStart();

	beep.init((OutputInitParam){"BEEP", PI11, GPIO_LEVEL_HIGH});

	//串口初始化
	uart1.Init((UartInitParam){"UART1", PA10, PA9, _UART1, 115200, protos, sizeof(protos) / sizeof(Protocol*), MsgDeal});

	// log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	Logger::SetTimeCallback(System::Time::getSysTime);
	LOG_INFO(LOG_CH_UART, "hello world !\r\n");

	beep.open();
	System::Time::delayMs(100);
	beep.close();

}