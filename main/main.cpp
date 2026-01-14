#include "APP.hpp"


int main(void)
{
	System::TimeMark tm;

	System::init();

	//启动任务
	TaskParam p1;
	t1.start(&p1, 0);

	while(1)
	{
		tm.insert();
		System::run();
		Task::run();
		if(tm.get() > 1000)//主循环大于1ms
		{
			
		}
	}
}