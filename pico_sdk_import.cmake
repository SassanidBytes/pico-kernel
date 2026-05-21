cmake_minimum_required(VERSION 3.13)

if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif()

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)