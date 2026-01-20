#pragma once
#include "dev.hpp" 


#define LOG(str, ...)		LOG_INFO(LOG_CH_UART, str, ##__VA_ARGS__);

class Task;

typedef enum//Task status
{
	TASK_STATE_IDEL,
	TASK_STATE_RUN,
	TASK_STATE_ERROR,
	TASK_STATE_DELAY,
	TASK_STATE_WAITEVENT,
	TASK_STATE_START,
	TASK_STATE_SUCCESS,
	TASK_STATE_FAIL,
	TASK_STATE_WAITSUBTASK,
} TaskState;

typedef struct
{
	Task* father = nullptr;
	void* data = nullptr;
	int dataLen = 0;
}TaskParam;

typedef void (*TaskFun)(Task* self, TaskParam* param) ;

typedef enum
{
	//0-255 Jump to userState

	WHERE_NEXT = 256,
	WHERE_NULL,
	WHERE_SUCCESS,
	WHERE_FAIL
}WhereToGO;

class Task
{
public:

	Task(const char* name, TaskFun fun);
	~Task();

	void start(uint8_t startUserState = 0);
	void start(TaskParam* param, uint8_t startUserState = 0);
	void stop();
	void success();
	void fail();
	void subtaskStart(Task* subtask, TaskParam* param, uint8_t startUser_state, WhereToGO successGo, WhereToGO failGo);
	void subtaskStart(Task* subtask, TaskParam* param, uint8_t startUser_state, WhereToGO successGo, WhereToGO failGo, uint32_t delay);
	void subtaskStart(Task* subtask, TaskParam* param, uint8_t startUser_state, WhereToGO successGo, WhereToGO failGo, uint32_t delay, uint32_t timeout);
	void delay(uint32_t ms, WhereToGO successGo);
	void userStateChange(uint8_t userState);
	void transitionToNextState(); 
	uint8_t getUserState();
	bool isTimeout(uint32_t ms, WhereToGO failGo);

	//Static methods
	static void run(uint64_t nowTime);//Called in System::run()
	static int getObjectCount();//Called in System::run()


private:
	//Basic properties
	const char* name;
	TaskState state;
	TaskFun fun;
	Task* father;
	TaskParam param;
	
	//Used to traverse all tasks
	static const int MAX_OBJECTS = 64;   // Maximum object count
	static Task* objects[MAX_OBJECTS]; // Static array to store object pointers
	static int objectCount;               // Current object count

	//State machine
	uint8_t userState;
	uint8_t lastUserState;//Previous state value
	
	//Task execution time
	uint32_t taskStartTime;
	uint64_t userStateIntoTime;//Time entering current state

	//Delay
	uint32_t delayStartTime;
	uint32_t delayTime;
	
	//Destination
	uint32_t finishDelay;
	uint32_t timeout;
	Task* subtask;
	WhereToGO successGo;
	WhereToGO failGo;
	
	//Private method
	void goTo(WhereToGO userState);
};