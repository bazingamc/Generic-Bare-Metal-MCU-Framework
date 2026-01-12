#include "frame.hpp"

//任务函数声明
void Task1(Task* self, TaskParam* param);
void Task2(Task* self, TaskParam* param);

//创建任务（推荐使用全局对象）
Task t1("t1", Task1);//创建任务1
Task t2("t2", Task2);//创建任务2


void Task1(Task* self, TaskParam* param)
{
	switch (self->getUserState())
	{
	case 0:
		self->transitionToNextState();//跳转到下一个状态
		break;
	case 1:
		self->delay(2000, WHERE_NEXT);//延迟2秒后跳转到下一个状态
		break;
	case 2:
		self->subtaskStart(&t2, param, 0, WHERE_NEXT, WHERE_FAIL);//启动子任务t2，成功后跳转到下一个状态，失败后跳转到失败状态
		break;
	case 3:
		self->userStateChange(0);//回到初始状态，重新开始

	default:
		break;
	}
}

void Task2(Task* self, TaskParam* param)
{
	switch (self->getUserState())
	{
	case 0:
		self->userStateChange(1);
		break;
	case 1:
		self->delay(2000, WHERE_NEXT);
		break;
	case 2:
		self->success();
		break;
	default:
		break;
	}
}

int main(void)
{


	hal_gpio.init(PA0, GPIO_Mode_IN);

	// 初始化 UART1，波特率 115200
    hal_uart.init(_UART2, 115200);
    
    // 发送字符串
    hal_uart.send_string(_UART1, "Hello World!\r\n");

	//启动任务
	TaskParam p1;
	t1.start(&p1, 0);

	while(1)
	{
		Task::run();
	}
}

