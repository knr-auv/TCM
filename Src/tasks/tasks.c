#include <stdbool.h>
#include <stdio.h>
#include "scheduler/scheduler.h"
#include "config.h"
#include "IO/LED.h"
#include "tasks_config.h"
#include "Sensors/analog_sensors.h"
#include "telemetry/telemetry_debug.h"
#include "operation/ControlLoop.h"
#include "operation/systemControl.h"

void taskFun1(timeUs_t t)
{
	LED_Green_Toggle();
}
void taskFun2(timeUs_t t)
{
	LED_Red_Toggle();
}
void TASK_read_analog_sensors(timeUs_t t)
{
	ANALOG_beginConversion();
}

task_t tasks[TASK_COUNT] = {
	[TASK_SYSTEM] =
		{
			.taskName = "SYSTEM",
			.taskFun = taskSystem,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_SYSTEM_PERIOD_HZ),
			.staticPriority = TASK_PRIORITY_LOW},
	[TASK_READ_ANALOG_SENSORS] =
		{
			.taskName = "READ_ANALOG_SENSORS",
			.taskFun = TASK_read_analog_sensors,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_READ_ANALOG_SENSORS_HZ),
			.staticPriority = TASK_PRIORITY_LOW},
	[TASK_TELEMETRY_DEBUG] =
		{
			.taskName = "TELEMETRY_DEBUG",
			.taskFun = TELEMETRY_DEBUG_task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_TELEMETRY_DEBUG_HZ),
			.staticPriority = TASK_PRIORITY_LOW},
	[TASK_PID] =
		{
			.taskName = "CONTROL_LOOP_TASK",
			.taskFun = CL_TaskFun,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_PID_HZ),
			.staticPriority = TASK_PRIORITY_REALTIME
		},

	[TASK_SYSTEM_CONTROL] =
		{
			.taskName = "SYSTEM CONTROL TASK",
			.taskFun = SYSCTRL_Task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_SYSTEM_CTRL_HZ),
			.staticPriority = TASK_PRIORITY_HIGH},
	[TASK_LED_TEST] = 
		{
			.taskName = "TASK_LED_TEST",
			.taskFun = taskFun1,
			.desiredPeriod = TASK_PERIOD_HZ(5),
			.staticPriority = TASK_PRIORITY_HIGH
		}
	};

void initTasks(void)
{
	initScheduler();
	enableTask(TASK_READ_ANALOG_SENSORS, true);
	enableTask(TASK_TELEMETRY_DEBUG, true);
	enableTask(TASK_PID, true);
	enableTask(TASK_SYSTEM_CONTROL, true);
	enableTask(TASK_LED_TEST, false);
	//enableTask(TASK_SYSTEM_CONTROL, true);
}