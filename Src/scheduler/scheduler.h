#pragma once
#include <stdint.h>
#include <stdbool.h>

#define TASK_PERIOD_KHZ(khz) (1000000000 / (khz))
#define TASK_PERIOD_HZ(hz) (1000000 / (hz))
#define TASK_PERIOD_MS(ms) ((ms) * 1000)
#define TASK_PERIOD_US(us) (us)
#define MAX(a, b) ((a>b)?a:b)

typedef uint64_t timeUs_t;
typedef uint32_t timeMs_t;

typedef enum {
	TASK_PRIORITY_IDLE = 1,
	TASK_PRIORITY_LOW = 2,
	TASK_PRIORITY_MEDIUM = 3,
	TASK_PRIORITY_HIGH = 4,
	TASK_PRIORITY_REALTIME = 5,
	TASK_PRIORITY_MAX = 255
}taskPriority_e;


typedef enum {
	TASK_SYSTEM,
	TASK_READ_ANALOG_SENSORS,
	TASK_TELEMETRY_DEBUG,
	TASK_CONTROL_LOOP,
	TASK_COMM_HANDLER,
	TASK_LED_TEST,
	TASK_AUTOMATIONS,
	TASK_DIRECT_MOTORS_CTRL,
	TASK_HEART_BEAT,
	TASK_COUNT
}taskID_e;

typedef struct {
	const char* taskName;										//for debuging purpose
	void (*taskFun)(timeUs_t currentTime);						//main task function
	bool (*checkFun)(timeUs_t currentTime, timeUs_t deltaTime);	//check function for event driven tasks
	taskPriority_e staticPriority;								//task priority
	uint16_t dynamicPriority;									//prevent task from not being executed
	timeUs_t lastExecution;		
	timeUs_t lastSignaled;
	timeUs_t desiredPeriod;
	timeUs_t maxExecutionTime;									//just for stats and debuging
	timeUs_t totalExecutionTime;
	timeUs_t movingSumExTime;									//moving sum of time consumed by 32 invokations
	uint32_t ageCycles;
}task_t;

extern task_t tasks[TASK_COUNT];
void taskSystem(timeUs_t currentTime);
void enableTask(taskID_e taskID, bool enabled);
void initScheduler(void);
void scheduler(void);
