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

scheduler/scheduler.c
tasks/tasks.c

flight/PID.c
flight/IMU.c

telemetry/telemetry_debug.c

interrupts.c

main.c

time/time.c
IO/LED.c
IO/thrusters.c
IO/controls.c


PARENT_SCOPE
)