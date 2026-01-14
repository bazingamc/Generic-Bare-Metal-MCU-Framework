#include "dev.hpp"

// 静态成员初始化
Input* Input::objects[Input::MAX_OBJECTS] = { nullptr };
int Input::objectCount = 0;

Input::Input()
{
	if (objectCount < MAX_OBJECTS) 
	{
		objects[objectCount++] = this;
	}
	else 
	{
		// 超过容量可打印警告或处理
	}

    this->state = InputState_Inactive;
    this->lastState = InputState_Inactive;
    
    // 初始化滤波计数器
    this->activeCounter = 0;
    this->inactiveCounter = 0;
    this->currentReading = false;
}

Input::~Input()
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

void Input::init(InputParam param)
{
	memcpy(&this->initParam, &param, sizeof(param));
	
	hal_gpio.init(this->initParam.pin, GPIO_IN);
    
    // 初始化滤波计数器
    this->activeCounter = 0;
    this->inactiveCounter = 0;
    
    // 根据当前状态设置初始状态
    this->currentReading = ((GpioLevel)hal_gpio.read(this->initParam.pin) == this->initParam.activeLevel);
    if(this->currentReading) {
        this->state = InputState_Active;
        this->activeCounter = this->initParam.filterCount;  // 立即达到激活阈值
    } else {
        this->state = InputState_Inactive;
        this->inactiveCounter = this->initParam.filterCount;  // 立即达到非激活阈值
    }
    this->lastState = this->state;
}


bool Input::isActive()
{
    return (this->state == InputState_Active);
}

bool Input::isInactive()
{
    return !isActive();
}

InputState Input::getState() const
{
    return this->state;
}

const char* Input::getName() const
{
    return this->initParam.name;
}

void Input::inputTask()
{
	for (uint16_t i = 0; i < Input::objectCount; i++)
	{
		Input* obj = Input::objects[i];
		
		// 读取当前GPIO状态
		GpioLevel currentLevel = (GpioLevel)hal_gpio.read(obj->initParam.pin);
		bool detectedActive = (currentLevel == obj->initParam.activeLevel);
		
		// 根据检测到的电平更新计数器
		if(detectedActive) {
			// 如果检测到激活电平，增加激活计数，重置非激活计数
			if(obj->activeCounter < obj->initParam.filterCount) {
				obj->activeCounter++;
			}
			obj->inactiveCounter = 0;
		} else {
			// 如果检测到非激活电平，增加非激活计数，重置激活计数
			if(obj->inactiveCounter < obj->initParam.filterCount) {
				obj->inactiveCounter++;
			}
			obj->activeCounter = 0;
		}
		
		// 检查是否达到了滤波阈值
		if(obj->activeCounter >= obj->initParam.filterCount) {
			// 达到激活阈值，更新状态为激活，并重置计数器
			obj->lastState = obj->state;
			obj->state = InputState_Active;
			obj->activeCounter = obj->initParam.filterCount;  // 锁定激活计数器到最大值
			obj->inactiveCounter = 0;  // 重置非激活计数器
		} else if(obj->inactiveCounter >= obj->initParam.filterCount) {
			// 达到非激活阈值，更新状态为非激活，并重置计数器
			obj->lastState = obj->state;
			obj->state = InputState_Inactive;
			obj->inactiveCounter = obj->initParam.filterCount;  // 锁定非激活计数器到最大值
			obj->activeCounter = 0;  // 重置激活计数器
		}
	}
}