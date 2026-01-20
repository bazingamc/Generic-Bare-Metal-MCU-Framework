#include "dev.hpp"

// Static member initialization
Input* Input::objects[Input::MAX_OBJECTS] = { nullptr };
int Input::objectCount = 0;

Input::Input() :
	state(InputState_Inactive), 
	lastState(InputState_Inactive), 
	activeCounter(0), 
	inactiveCounter(0), 
	currentReading(false)
{
	if (objectCount < MAX_OBJECTS) 
	{
		objects[objectCount++] = this;
	}
	else 
	{
		// If capacity exceeded, print warning or handle
	}
}

Input::~Input()
{
	for (int i = 0; i < objectCount; i++) 
	{
		if (objects[i] == this) 
		{
			// Replace the current position with the last object
			objects[i] = objects[objectCount - 1];
			objects[objectCount - 1] = nullptr;
			objectCount--;
			break;
		}
	}
}

void Input::init(InputInitParam param)
{
	memcpy(&this->initParam, &param, sizeof(param));
	
	hal_gpio.init(this->initParam.pin, GPIO_IN);
    
    // Initialize filter counters
    this->activeCounter = 0;
    this->inactiveCounter = 0;
    
    // Set initial state based on current state
    this->currentReading = ((GpioLevel)hal_gpio.read(this->initParam.pin) == this->initParam.activeLevel);
    if(this->currentReading) {
        this->state = InputState_Active;
        this->activeCounter = this->initParam.filterCount;  // Immediately reach activation threshold
    } else {
        this->state = InputState_Inactive;
        this->inactiveCounter = this->initParam.filterCount;  // Immediately reach deactivation threshold
    }
    this->lastState = this->state;
}

int Input::getObjectCount()
{
    return Input::objectCount;
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
		
		// Read current GPIO state
		GpioLevel currentLevel = (GpioLevel)hal_gpio.read(obj->initParam.pin);
		bool detectedActive = (currentLevel == obj->initParam.activeLevel);
		
		// Update counter based on detected level
		if(detectedActive) {
			// If active level is detected, increase active count, reset inactive count
			if(obj->activeCounter < obj->initParam.filterCount) {
				obj->activeCounter++;
			}
			obj->inactiveCounter = 0;
		} else {
			// If inactive level is detected, increase inactive count, reset active count
			if(obj->inactiveCounter < obj->initParam.filterCount) {
				obj->inactiveCounter++;
			}
			obj->activeCounter = 0;
		}
		
		// Check if filter threshold is reached
		if(obj->activeCounter >= obj->initParam.filterCount) {
			// Reached activation threshold, update state to active and reset counters
			obj->lastState = obj->state;
			obj->state = InputState_Active;
			obj->activeCounter = obj->initParam.filterCount;  // Lock active counter to maximum value
			obj->inactiveCounter = 0;  // Reset inactive counter
		} else if(obj->inactiveCounter >= obj->initParam.filterCount) {
			// Reached deactivation threshold, update state to inactive and reset counters
			obj->lastState = obj->state;
			obj->state = InputState_Inactive;
			obj->inactiveCounter = obj->initParam.filterCount;  // Lock inactive counter to maximum value
			obj->activeCounter = 0;  // Reset active counter
		}
	}
}