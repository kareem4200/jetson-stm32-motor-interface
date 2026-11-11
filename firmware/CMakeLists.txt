cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)         # skip the compiler build check
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/target.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

project(motor_interface C ASM)

set(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_ASM_FLAGS_DEBUG "-g3")

add_executable(firmware
    app/main.c
    startup/startup_stm32f401xx.S
    startup/syscall.c
    startup/sysmem.c
)

target_include_directories(firmware PRIVATE
    third_party/cmsis_core
    third_party/cmsis_device_f4/include
    drivers/include
)

target_compile_definitions(firmware PRIVATE
    STM32F401xC
    RTE_DEVICE_STARTUP_STM32F4XX
)

target_compile_options(firmware PRIVATE
    ${CPU_FLAGS}
    -std=gnu11 -Wall -Wextra -Wpedantic -Wno-unused-parameter
)

target_link_options(firmware PRIVATE
    ${CPU_FLAGS}
    -T${CMAKE_CURRENT_SOURCE_DIR}/startup/stm32f401xc_flash.ld
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/firmware.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    --specs=nosys.specs
)

set_target_properties(firmware PROPERTIES
    LINK_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/startup/stm32f401xc_flash.ld)

# make the .bin file after linking
add_custom_command(TARGET firmware POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:firmware> ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:firmware>)