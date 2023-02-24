#include <stdbool.h>
#include <stdio.h>
#include "scheduler/scheduler.h"
#include "config.h"
#include "IO/LED.h"
#include "config_tasks.h"

#include "Sensors/MS5837-30BA/depth_sensor.h"
#include "operation/ControlLoop.h"
#include "communication/CommunicationHandler.h"
#include "operation/Automations.h"
#include "operation/DirectThrustersCtrl.h"
#include "HeartBeat.h"
#include "Variables/variables.h"
#include "tasks.h"

void taskFun1(timeUs_t t)
{
	LED_Green_Toggle();
}
void taskFun2(timeUs_t t)
{
	LED_Red_Toggle();
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
	[TASK_TELEMETRY] =
		{
			.taskName = "TELEMETRY",
			.taskFun = TASK_Telemetry,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_TELEMETRY_HZ),
			.staticPriority = TASK_TELEMETRY_PRIORITY},
	[TASK_CONTROL_LOOP] =
		{
			.taskName = "CONTROL_LOOP_TASK",
			.taskFun = CL_TaskFun,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_PID_HZ),
			.staticPriority = TASK_PRIORITY_REALTIME},

	[TASK_COMM_HANDLER] =
		{
			.taskName = "COMM HANDLER TASK",
			.taskFun = COMHANDLER_Task,
			.checkFun = COMHANDLER_CheckFun,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_COMM_HANDLER_HZ),
			.staticPriority = TASK_PRIORITY_REALTIME},
	[TASK_LED_TEST] =
		{
			.taskName = "TASK_LED_TEST",
			.taskFun = taskFun1,
			.desiredPeriod = TASK_PERIOD_HZ(5),
			.staticPriority = TASK_PRIORITY_HIGH},
	[TASK_AUTOMATIONS] =
		{
			.taskName = "TASK_AUTOMATIONS",
			.taskFun = AUTOMATIONS_Task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_AUTOMATIONS_HZ),
			.staticPriority = TASK_PRIORITY_HIGH},
	[TASK_DIRECT_MOTORS_CTRL] =
		{
			.taskName = "TASK_DIRECT_MOTORS_CTRL",
			.taskFun = DTCTRL_Task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_DIRECT_MOTORS_CTRL_HZ),
			.staticPriority = TASK_PRIORITY_HIGH},
	[TASK_HEART_BEAT] =
		{
			.taskName = "TASK_HEART_BEAT",
			.taskFun = HB_Task,
			.desiredPeriod = TASK_PERIOD_HZ(TASK_HEART_BEAT_HZ),
			.staticPriority = TASK_PRIORITY_HIGH},
	[TASK_READ_DEPTH_SENSOR] =
		{
			.taskName = "READ_DEPTH_SENSOR",
			.taskFun = read_depth_sensor,
			.checkFun = NULL, // it will be set in .taskFun
			.desiredPeriod = TASK_PERIOD_HZ(TASK_READ_DEPTH_SENSOR_HZ),
			.staticPriority = TASK_PRIORITY_HIGH},

};

void initTasks(void)
{
	initScheduler();

	enableTask(TASK_READ_ANALOG_SENSORS, true);
	enableTask(TASK_TELEMETRY, true);
	enableTask(TASK_CONTROL_LOOP, true);
	enableTask(TASK_COMM_HANDLER, true);
	enableTask(TASK_LED_TEST, false);
	enableTask(TASK_AUTOMATIONS, true);
	enableTask(TASK_DIRECT_MOTORS_CTRL, false);
	enableTask(TASK_HEART_BEAT, true);
	//enableTask(TASK_READ_DEPTH_SENSOR, true);
}