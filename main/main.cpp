#include "APP.hpp"

Output led1;
int main(void)
{
	System::TimeMark tm;
	System::init();

	OutputInitParam param;
	param.name = "LED1";
	param.pin = PH10;
	param.validLevel = GPIO_LEVEL_LOW;
	led1.init(param);
	led1.pulseOutputStart();//开始脉冲输出，默认参数为：周期1000ms，占空比50%

	while(1)
	{
		System::run();
	}
}