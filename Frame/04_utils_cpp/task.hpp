#pragma once
#include "dev.hpp" 


#define LOG(str, ...)		//printf(str, ##__VA_ARGS__);

class Task;

typedef enum//任务状态
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
	//0-255 跳转userState

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

	//静态方法
	static void run();//System::run()中调用
	static int getObjectCount();//System::run()中调用

	//系统时间
	static uint64_t nowTime;

private:
	//基本属性
	const char* name;
	TaskState state;
	TaskFun fun;
	Task* father;
	TaskParam param;
	
	//用于遍历所有任务
	static const int MAX_OBJECTS = 64;   // 最大对象数量
	static Task* objects[MAX_OBJECTS]; // 静态数组保存对象指针
	static int objectCount;               // 当前对象数量

	//状态机
	uint8_t userState;
	uint8_t lastUserState;//上一次状态值
	
	//任务执行时间
	uint32_t taskStartTime;
	uint64_t userStateIntoTime;//进入当前状态的时间

	//延迟
	uint32_t delayStartTime;
	uint32_t delayTime;
	
	//去向
	uint32_t finishDelay;
	uint32_t timeout;
	Task* subtask;
	WhereToGO successGo;
	WhereToGO failGo;
	
	//私有方法
	void goTo(WhereToGO userState);
};