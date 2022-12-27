cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(bopitbomb)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(bopit
    src/main.cpp
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(bopit pico_stdlib hardware_adc pico_printf)

pico_enable_stdio_usb(bopit 1)
pico_enable_stdio_uart(bopit 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(bopit)

add_custom_target(deploy ./pico-upload
  WORKING_DIRECTORY ./) 
