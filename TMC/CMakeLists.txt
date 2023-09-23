set(SOURCES

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

    Common/quaternions.c
    Common/PID.c
    Common/common.c

    Config/config.c
    scheduler/scheduler.c

    tasks/tasks.c
    tasks/HeartBeat.c
    tasks/task_Telemetry.c
    tasks/task_ReadAnalogSensors.c

    Config/Limits.c
    Flash/flash.c

    operation/ControlLoop.c
    operation/IMU.c
    operation/DirectThrustersCtrl.c
    operation/Automations.c
    operation/ServiceMode.c

    Variables/variables.c

    Communication/CommunicationHandler.c
    Communication/protocol.c


    interrupts.c
    time/time.c

    IO/LED.c
    IO/actuators.c

    IO/Sticks.c
    startup/startup_stm32f407xx.s
    startup/system_stm32f4xx.c
    startup/syscalls.c
    startup/sysmem.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/startup/STM32F407VGTx_FLASH.ld)

target_link_options(${PROJECT_NAME} PRIVATE -T${LINKER_SCRIPT})
target_link_options(${PROJECT_NAME} PRIVATE -Wl,-Map=${PROJECT_NAME}.map)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf") 

# add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
# )

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
    $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
    $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -d
    $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.elf > ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.txt
)
target_link_directories("${PROJECT_NAME}" PUBLIC ${CMAKE_SOURCE_DIR})
target_compile_definitions("${PROJECT_NAME}" PUBLIC BOOTLOADER_VERSION_MAJOR=${VERSION_MAJOR} BOOTLOADER_VERSION_MINOR=${VERSION_MINOR})
target_include_directories("${PROJECT_NAME}" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories("${PROJECT_NAME}" PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})