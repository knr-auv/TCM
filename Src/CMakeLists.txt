set(MAIN_SRC
setup.c
drivers/GPIO.c
drivers/USART1.c
drivers/USART3.c
drivers/USART2.c
drivers/SPI3.c

Sensors/BNO080/bno080.c
Sensors/BNO080/hw.c
Sensors/UM7/UM7.c
Sensors/analog_sensors.c

helpers/quaternions.c
helpers/PID.c
helpers/common.c
helpers/comunication_protocol.c

scheduler/scheduler.c
tasks/tasks.c

Config/Limits.c

operation/ControlLoop.c
operation/IMU.c
operation/systemControl.c
operation/DirectThrustersCtrl.c

telemetry/telemetry_debug.c

interrupts.c

main.c

time/time.c
IO/LED.c
IO/thrusters.c

IO/Sticks.c


PARENT_SCOPE
)