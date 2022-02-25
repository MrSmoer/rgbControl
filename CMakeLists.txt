cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(rgbControl)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(rgbControl
    rgbControl.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(rgbControl pico_stdlib pico_bootsel_via_double_reset pico_multicore hardware_pwm)

# enable usb output, disable uart output
pico_enable_stdio_usb(rgbControl 1)
pico_enable_stdio_uart(rgbControl 0)


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(rgbControl)
