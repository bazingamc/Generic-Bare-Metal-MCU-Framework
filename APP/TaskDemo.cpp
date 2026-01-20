#include "APP.hpp"




// Create tasks (recommended to use global objects)
Task t1("t1", Task1);// Create task 1
Task t2("t2", Task2);// Create task 2


void Task1(Task* self, TaskParam* param)
{
	switch (self->getUserState())
	{
	case 0:
		self->transitionToNextState();// Transition to next state
		break;
	case 1:
		self->delay(2000, WHERE_NEXT);// Delay for 2 seconds then transition to next state
		break;
	case 2:
		// Start subtask t2, transition to next state on success, failure state on failure
		self->subtaskStart(&t2, param, 0, WHERE_NEXT, WHERE_FAIL);
		break;
	case 3:
		self->userStateChange(0);// Return to initial state and restart

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