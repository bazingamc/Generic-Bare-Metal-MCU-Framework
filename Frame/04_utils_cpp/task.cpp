#include "task.hpp"

// Static member initialization
Task* Task::objects[Task::MAX_OBJECTS] = { nullptr };
int Task::objectCount = 0;


Task::Task(const char* name, TaskFun fun)
{
	this->fun = fun;
	this->state = TASK_STATE_IDEL;
	this->name = name;

	if (objectCount < MAX_OBJECTS) 
	{
		objects[objectCount++] = this;
	}
	else 
	{
		// If capacity exceeded, can print warning or handle
		//cout << "Warning: Max object limit reached!" << endl;
	}
}

Task::~Task()
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

int Task::getObjectCount()
{
    return Task::objectCount;
}

void Task::start(uint8_t start_userState)
{
	this->state = TASK_STATE_START;
	this->userState = start_userState;
}
void Task::start(TaskParam* param, uint8_t start_userState)
{
	this->state = TASK_STATE_START;
	memcpy(&this->param, param, param->dataLen);
	this->userState = start_userState;
}

void Task::stop()
{
	this->state = TASK_STATE_IDEL;
}

void Task::subtaskStart(Task* subtask, TaskParam* param, uint8_t start_userState, WhereToGO successGo, WhereToGO failGo)
{
	this->subtask = subtask;
	this->state = TASK_STATE_WAITSUBTASK;
	this->subtask->start(param, start_userState);
	this->finishDelay = 0;
	this->timeout = 0;
}

void Task::subtaskStart(Task* subtask, TaskParam* param, uint8_t start_userState, WhereToGO successGo, WhereToGO failGo, uint32_t delay)
{
	this->subtask = subtask;
	this->state = TASK_STATE_WAITSUBTASK;
	this->subtask->start(param, start_userState);
	this->finishDelay = delay;
	this->timeout = 0;
}

void Task::subtaskStart(Task* subtask, TaskParam* param, uint8_t start_userState, WhereToGO successGo, WhereToGO failGo, uint32_t delay, uint32_t timeout)
{
	this->subtask = subtask;
	this->state = TASK_STATE_WAITSUBTASK;
	this->subtask->start(param, start_userState);
	this->finishDelay = delay;
	this->timeout = timeout;
}

void Task::success()
{
	this->state = TASK_STATE_SUCCESS;
}

void Task::fail()
{
	this->state = TASK_STATE_FAIL;
}

void Task::delay(uint32_t ms, WhereToGO successGo)
{
	this->delayTime = ms;
	this->delayStartTime = System::Time::getSysTime();
	this->state = TASK_STATE_DELAY;
	this->successGo = successGo;
}

void Task::userStateChange(uint8_t userState)
{
	this->userState = userState;
}
void Task::transitionToNextState()
{
	this->userState++;
}
uint8_t Task::getUserState()
{
	return this->userState;	
}
bool Task::isTimeout(uint32_t ms, WhereToGO failGo)
{
	if ((System::Time::getSysTime() - this->userStateIntoTime) >= ms)
	{
		this->goTo(failGo);
		return true;
	}
	return false;
}

void Task::goTo(WhereToGO userState)
{
	if (userState < 256)
	{
		this->userStateChange(userState);
	}
	else
	{
		switch (userState)
		{
		case WHERE_NEXT:
			this->transitionToNextState();
			break;
		case WHERE_SUCCESS:
			this->success();
			break;
		case WHERE_FAIL:
			this->fail();
			break;
		default:
			// Do nothing
			break;
		}
	}
}

void Task::run(uint64_t nowTime)
{
	for (uint16_t i = 0; i < Task::objectCount; i++)
	{
		Task* obj = Task::objects[i];
		switch (obj->state)
		{
		case TASK_STATE_START:
			obj->state = TASK_STATE_RUN;
			obj->taskStartTime = nowTime;
			if (obj->father)
			{
				LOG("T:%s start, Father:%s\r\n", obj->name, obj->father->name);
			}
			else
			{
				LOG("T:%s start\r\n", obj->name);
			}
			break;
		case TASK_STATE_RUN:
			if (obj->lastUserState != obj->userState)
			{
				LOG("T:%s user state: %d -> %d\r\n", obj->name, obj->lastUserState, obj->userState);
				obj->lastUserState = obj->userState;
				obj->userStateIntoTime = nowTime;
			}
			obj->fun(obj, &obj->param);
			break;
		case TASK_STATE_DELAY:
			if (nowTime - obj->delayStartTime >= obj->delayTime)
			{
				obj->state = TASK_STATE_RUN;
				obj->goTo(obj->successGo);
			}
			break;
		case TASK_STATE_WAITSUBTASK:
			if (obj->subtask->state == TASK_STATE_IDEL)
			{
				if (obj->finishDelay)
				{
					obj->delay(obj->finishDelay, obj->successGo);
				}
				else
				{
					obj->state = TASK_STATE_RUN;
					obj->goTo(obj->successGo);
				}
			}
			else if (obj->subtask->state == TASK_STATE_FAIL)
			{
				if (obj->finishDelay)
				{
					obj->delay(obj->finishDelay, obj->failGo);
				}
				else
				{
					obj->state = TASK_STATE_RUN;
					obj->goTo(obj->failGo);
				}
			}
			else if (obj->timeout != 0 && (nowTime - obj->userStateIntoTime) >= obj->timeout)
			{
				LOG("T:%s subtask %s timeout\r\n", obj->name, obj->subtask->name);
				obj->state = TASK_STATE_RUN;
				obj->goTo(obj->failGo);
			}
			break;
		case TASK_STATE_SUCCESS:
			LOG("T:%s finish, time %dms\r\n", obj->name, nowTime - obj->taskStartTime);
			obj->state = TASK_STATE_IDEL;
			break;
		case TASK_STATE_FAIL:
			LOG("T:%s fail, time %dms\r\n", obj->name, nowTime - obj->taskStartTime);
			obj->state = TASK_STATE_ERROR;
			break;

		default:
			break;
		}
	}
}