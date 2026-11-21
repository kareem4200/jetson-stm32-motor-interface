cmake_minimum_required(VERSION 3.20)
project(test LANGUAGES C)

enable_testing()

SET(TEST_INCLUDE_DIRS
    ../../3rd_party/cmsis_core
    ../../3rd_party/cmsis_device_f4/include
    ../drivers/include
    ../devices/include
    ../bsp/include
)

add_library(unity STATIC
    ../../3rd_party/Unity/src/unity.c
)

target_include_directories(unity PUBLIC
    ../../3rd_party/Unity/src
)

function(add_firmware_test test_name)
    add_executable(${test_name} ${ARGN})
    target_include_directories(${test_name} PRIVATE ${TEST_INCLUDE_DIRS})
    target_link_libraries(${test_name} PRIVATE unity)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_firmware_test(pwm unit/test_pwm.c
    ../drivers/baremetal/pwm.c
    ../drivers/baremetal/clock.c
)

add_firmware_test(gpio unit/test_gpio.c
    ../drivers/baremetal/gpio.c
)
