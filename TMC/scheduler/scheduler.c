#include <string.h>
#include <stdbool.h>
#include "IO/LED.h"
#include "scheduler.h"
#include "time/time.h"
#include "Config/config.h"
#ifdef SYSTEM_HEART_BEAT_ENABLE
#include "operation/CommunicationHandler.h"
#endif

task_t *taskQueue[TASK_COUNT + 1]; // extra item for null pointer at the end
task_t *currentTask = NULL;
static uint8_t taskQueueSize = 0;
static uint8_t taskQueuePos = 0;

static uint32_t totalWaitingTasks;
static uint32_t totalWaitingTasksSamples;
uint16_t averageSystemLoadPercent;

void taskSystem(timeUs_t currentTime)
{
	if (totalWaitingTasksSamples > 0)
	{
		averageSystemLoadPercent = 100 * totalWaitingTasks / totalWaitingTasksSamples;
		totalWaitingTasksSamples = 0;
		totalWaitingTasks = 0;
	}
}

static void clearQueue(void)
{
	memset(taskQueue, 0, sizeof(taskQueue));
	taskQueuePos = 0;
	taskQueueSize = 0;
}

static bool isInQueue(task_t *task)
{
	for (uint8_t i = 0; i < taskQueueSize; i++)
		if (taskQueue[i] == task)
			return true;
	return false;
}

static bool addToQueue(task_t *task)
{
	if (isInQueue(task) || taskQueueSize >= TASK_COUNT) // make sure that we have place
		return false;
	for (uint8_t i = 0; i <= taskQueueSize; i++)
	{
		if (taskQueue[i] == NULL)
		{
			memmove(&taskQueue[i + 1], &taskQueue[i], sizeof(task) * (taskQueueSize - i));
			taskQueue[i] = task;
			taskQueueSize++;
			return true;
		}
	}
	return false;
}

static bool queueRemove(task_t *task)
{
	for (int i = 0; i < taskQueueSize; i++)
	{
		if (taskQueue[i] == task)
		{
			memmove(&taskQueue[i], &taskQueue[i + 1], sizeof(task) * (taskQueueSize - i));
			taskQueueSize--;
			return true;
		}
	}
	return false;
}

static task_t *queueFirst(void)
{
	taskQueuePos = 0;
	return taskQueue[0];
}

static task_t *queueNext(void)
{
	return taskQueue[++taskQueuePos];
}

void enableTask(taskID_e taskID, bool enabled)
{
	if (taskID < TASK_COUNT)
	{
		task_t *task = &tasks[taskID];
		enabled ? addToQueue(task) : queueRemove(task);
	}
}
void initScheduler(void)
{
	clearQueue();
	enableTask(TASK_SYSTEM, true);
}
void scheduler(void)
{
	const timeUs_t currentTime = micros();
	task_t *selectedTask = NULL;
	uint16_t selectedTaskDynamicPriority = 0;
	uint8_t waitingTasks = 0;
	bool realTimeTaskLock = false;
	for (task_t *task = queueFirst(); task != NULL; task = queueNext())
	{
		if (task->checkFun) // if checkFun exist
		{
			const timeUs_t currentTimeBeforeCheckFuncCallUs = micros();
			if (task->dynamicPriority > 0)
			{
				task->ageCycles = 1 + ((timeDelta_t)(currentTime - task->lastSignaled)) / task->desiredPeriod;
				task->dynamicPriority = 1 + task->staticPriority * task->ageCycles;
				waitingTasks++;
			}
			else if (task->checkFun(currentTimeBeforeCheckFuncCallUs, currentTimeBeforeCheckFuncCallUs - task->lastExecution))
			{

				task->lastSignaled = currentTimeBeforeCheckFuncCallUs;
				task->ageCycles = 1;
				task->dynamicPriority = 1 + task->staticPriority;
				waitingTasks++;
			}
			else
			{
				task->ageCycles = 0;
			}
		}
		else if (task->staticPriority == TASK_PRIORITY_REALTIME) // handling high priority task
		{
			if ((currentTime - task->lastExecution) > task->desiredPeriod)
			{
				selectedTask = task;
				waitingTasks++;
				selectedTaskDynamicPriority = task->dynamicPriority;
				realTimeTaskLock = true;
			}
		}
		else // low priority tasks
		{
			task->ageCycles = (currentTime - task->lastExecution) / task->desiredPeriod;
			if (task->ageCycles > 0)
			{
				task->dynamicPriority = 1 + task->staticPriority * task->ageCycles;
				waitingTasks++;
			}
		}

		if (!realTimeTaskLock && (task->dynamicPriority > selectedTaskDynamicPriority)) // if there is no real time
		{
			selectedTaskDynamicPriority = task->dynamicPriority;
			selectedTask = task;
			waitingTasks++;
		}
	}

	totalWaitingTasks += waitingTasks;
	totalWaitingTasksSamples++;
	currentTask = selectedTask;
	if (selectedTask)
	{
		const timeUs_t timeBeforeTask = micros();
		selectedTask->taskFun(micros());
		const timeUs_t timeAfterTask = micros();
		timeUs_t dt = timeAfterTask - timeBeforeTask;
		selectedTask->totalExecutionTime += dt;
		selectedTask->maxExecutionTime = MAX(dt, selectedTask->maxExecutionTime);
		selectedTask->movingSumExTime += dt - selectedTask->movingSumExTime / 32;
		selectedTask->lastExecution = currentTime;
		selectedTask->dynamicPriority = 0;
	}
}