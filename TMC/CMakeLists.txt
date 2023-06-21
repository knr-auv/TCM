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
    tasks/task_Telemetry.c
    tasks/task_ReadAnalogSensors.c

    Config/Limits.c
    Memory/memory.c

    operation/ControlLoop.c
    operation/IMU.c
    operation/DirectThrustersCtrl.c
    operation/Automations.c
    operation/ServiceMode.c

    Variables/variables.c

    communication/CommunicationHandler.c
    communication/tx_handler.c
    communication/rx_handler.c


    interrupts.c
    time/time.c

    IO/LED.c
    IO/actuators.c

    IO/Sticks.c
)

macro(main_sources var) 
    set(${var} ${ARGN})
    list(TRANSFORM ${var} PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
endmacro()

main_sources(src ${MAIN_SRC})
set(MAIN_SOURCES ${src} PARENT_SCOPE)