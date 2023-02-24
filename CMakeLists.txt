cmake_minimum_required(VERSION 3.16)
set(CMAKE_SYSTEM_NAME Generic)
set(PROJECT_NAME TMC)
project(${PROJECT_NAME} C ASM)
set(CMAKE_C_STANDARD 17)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})
set(TARGET "main")

set(TARGET_ELF "${TARGET}.elf")
set(TARGET_HEX "${TARGET}.hex")
set(TARGET_BIN "${TARGET}.bin")


set(MAIN_SRC_DIR "${CMAKE_SOURCE_DIR}/Src")

set(LD_include "-lc -lm")

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld)

set(COMPILE_DEFINITIONS ARM_MATH_CM4 __FPU_PRESENT)
set(MCU_flags -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb)
set(COMPILE_FLAGS ${MCU_flags} -g3 -ffunction-sections -fdata-sections -Wall -fstack-usage -O0)
set(LD_FLAGS ${MCU_flags} ${LIBS} -Wl,--print-memory-usage -Wl,-Map=${BUILD_DIR}/main.map -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs)


add_subdirectory("${MAIN_SRC_DIR}")

add_executable(${TARGET_ELF}
    Drivers/CMSIS/startup_stm32f407xx.s
    Drivers/CMSIS/system_stm32f4xx.c
    #Drivers/CMSIS/syscalls.c
    #Drivers/CMSIS/sysmem.c
    "${MAIN_SOURCES}")

target_include_directories(${TARGET_ELF} PUBLIC
    Src/Std_Periph/inc
    Src
    Drivers/CMSIS/Device/ST/STM32F4xx/IncludeLD_FLAGS
    Drivers/CMSIS/Include)

    
target_compile_options(${TARGET_ELF} PRIVATE ${COMPILE_FLAGS})
target_compile_definitions(${TARGET_ELF} PRIVATE ${COMPILE_DEFINITIONS})
target_link_options(${TARGET_ELF} PRIVATE ${LD_FLAGS} -T${LINKER_SCRIPT})
target_link_libraries(${TARGET_ELF} PUBLIC "${CMAKE_SOURCE_DIR}/Drivers/CMSIS/libarm_cortexM4lf_math.a")
target_link_libraries(${TARGET_ELF} PUBLIC c m)

set(OBJCOPY arm-none-eabi-objcopy)
add_custom_target(${TARGET_BIN} ALL COMMAND ${OBJCOPY} -O binary -S ${BUILD_DIR}/${TARGET_ELF} ${BUILD_DIR}/${TARGET_BIN})
add_custom_target(${TARGET_HEX} ALL COMMAND ${OBJCOPY} -O ihex -S ${BUILD_DIR}/${TARGET_ELF} ${BUILD_DIR}/${TARGET_HEX})

add_dependencies(${TARGET_BIN} ${TARGET_ELF} ${TARGET_HEX})
