#include "APP.hpp"


int main(void)
{
	System::TimeMark tm;

	System::init();

	//启动任务
	t1.start();

	while(1)
	{
		tm.insert();
		System::run();
		if(tm.get() > 1000)//主循环大于1ms
		{
			
		}
	}
}