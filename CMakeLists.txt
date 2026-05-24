cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico_kernel C CXX ASM)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(pico_kernel
    src/main.c
    src/drivers/st7789.c
    src/kernel/shell.c
    src/kernel/tasks/scheduler.c
    src/kernel/mutex.c
    src/kernel/pong.c
    src/kernel/invaders.c
    src/kernel/passmanager.c
    src/kernel/totp.c
    src/kernel/storage.c
)

target_include_directories(pico_kernel PRIVATE
    src/drivers
    src/kernel
    src/kernel/tasks
    src/font
)

target_link_libraries(pico_kernel
    pico_stdlib
    hardware_spi
    hardware_gpio
    hardware_timer
    hardware_flash
    hardware_sync
    pico_multicore
)

pico_enable_stdio_usb(pico_kernel 1)
pico_enable_stdio_uart(pico_kernel 0)
pico_add_extra_outputs(pico_kernel)