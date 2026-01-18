#include "dev.hpp"
// 静态成员初始化
Output* Output::objects[Output::MAX_OBJECTS] = { nullptr };
int Output::objectCount = 0;

Output::Output() : duty(0), cycle(0), state(OutState_Close)
{
	if (objectCount < MAX_OBJECTS) 
	{
		objects[objectCount++] = this;
	}
	else 
	{
		// 超过容量可打印警告或处理
	}
}

Output::~Output()
{
	for (int i = 0; i < objectCount; i++) 
	{
		if (objects[i] == this) 
		{
			// 将最后一个对象替换到当前位置
			objects[i] = objects[objectCount - 1];
			objects[objectCount - 1] = nullptr;
			objectCount--;
			break;
		}
	}
}

void Output::init(OutputInitParam param)
{
	memcpy(&this->initParam, &param, sizeof(param));
    this->duty = 0;
    this->cycle = 0;
	hal_gpio.init(this->initParam.pin, GPIO_OUT);
    this->close();
}

int Output::getObjectCount()
{
    return Output::objectCount;
}

void Output::open()
{
    if(this->initParam.validLevel == GPIO_LEVEL_HIGH)
    {
        hal_gpio.write(this->initParam.pin, GPIO_LEVEL_HIGH);
    }
    else
    {
        hal_gpio.write(this->initParam.pin, GPIO_LEVEL_LOW);
    }
	return;
}

void Output::close()
{
    if(this->initParam.validLevel == GPIO_LEVEL_HIGH)
    {
        hal_gpio.write(this->initParam.pin, GPIO_LEVEL_LOW);
    }
    else
    {
        hal_gpio.write(this->initParam.pin, GPIO_LEVEL_HIGH);
    }
	return;
}

void Output::pulseOutputStart(u32 cycle , float duty)
{
	if(duty > 100) duty = 100;
	if(duty < 0) duty = 0;

	this->cycle = cycle;
    this->duty = duty;
	return;
}

void Output::pulseOutputStop()
{
    this->duty = 0;
	this->close();
	return;
}

const char* Output::getName() const
{
    return this->initParam.name;
}

OutState Output::getState() const
{
    return this->state;
}

float Output::getDuty() const
{
    return this->duty;
}

void Output::outputTask()
{
	for (uint16_t i = 0; i < Output::objectCount; i++)
	{
		Output* obj = Output::objects[i];
		if(obj->cycle)
		{
			long long  time;
			// gst(time);
			uint32_t t = time % obj->cycle;
			if(t < obj->cycle * (obj->duty / 100.0))//有效
			{
				obj->open();
			}
			else//无效
			{
				obj->close();
			}
		}
	}

}