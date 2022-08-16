set(MAIN_SRC

    main.c

    setup.c

    drivers/GPIO.c
    drivers/USART1.c
    drivers/USART3.c
    drivers/USART2.c
    drivers/USART.c
    drivers/SPI1.c
    drivers/SPI2.c
    drivers/SPI3.c
    drivers/I2C2.c

    drivers/SST25_flash/SST25_flash.c
    drivers/SST25_flash/SST25_platform.c

    Sensors/BNO080/bno080.c
    Sensors/BNO080/hw.c
    Sensors/UM7/UM7.c

    Sensors/MS5837-30BA/depth_sensor.c
    Sensors/analog_sensors.c

    helpers/quaternions.c
    helpers/PID.c
    helpers/common.c
    helpers/CommunicationProtocol/communication_protocol.c

    scheduler/scheduler.c

    tasks/tasks.c
    tasks/HeartBeat.c

    Config/Limits.c
    Config/memory.c

    operation/ControlLoop.c
    operation/IMU.c
    operation/CommunicationHandler.c
    operation/DirectThrustersCtrl.c
    operation/Automations.c
    operation/ServiceMode.c

    Variables/variables.c

    telemetry/telemetry_debug.c

    interrupts.c
    time/time.c

    IO/LED.c
    IO/actuators.c

    IO/Sticks.c
    PARENT_SCOPE
)