#include "APP.hpp"

Output led1;

AsciiProtocol* protos[] = {&default_proto};
Uart uart1(1024, 1024, protos, 1);

int main(void)
{
	System::init();

	//程序运行指示灯
	OutputInitParam param;
	param.name = "LED1";
	param.pin = PH10;
	param.validLevel = GPIO_LEVEL_LOW;
	led1.init(param);
	led1.pulseOutputStart();//开始脉冲输出，默认参数为：周期1000ms，占空比50%

	//串口
	UartInitParam uartParam;
	uartParam.name = "UART1";
	uartParam.uart = _UART1;
	uartParam.baudrate = 115200;
	uart1.Init(uartParam);

	//log
	Logger::RegisterChannel(LOG_CH_UART, &uart1);
	LOG_INFO(LOG_CH_UART, "hello world !");

	while(1)
	{
		System::run();
	}
}