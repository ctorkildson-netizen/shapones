# ShapoNES for the ClockworkPi PicoCalc (Pico 2 / Pico 2 W, RP2350)
cmake_minimum_required(VERSION 3.13)

# NES_BT=ON: the Bluetooth build, shapones_picocalc_bt, for a Pico 2 W: a
# gamepad and keyboard paired with btpair (picocalc-extras, a submodule in
# modules/; it needs the Pico SDK's newer BTstack, as picocalc-extras says).
option(NES_BT "Bluetooth gamepad and keyboard (Pico 2 W)" OFF)
if (NES_BT)
    set(PICO_BOARD pico2_w CACHE STRING "Board type" FORCE)
else()
    set(PICO_BOARD pico2 CACHE STRING "Board type")
endif()
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

if (NES_BT)
    set(APP_NAME shapones_picocalc_bt)
else()
    set(APP_NAME shapones_picocalc)
endif()
project(${APP_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${APP_NAME})
add_compile_definitions(SHAPONES_ENABLE_LOG=0)
option(NES_DEBUG_ROW "a row of squares above the picture: what the emulator receives" OFF)
if (NES_DEBUG_ROW)
    add_compile_definitions(NES_DEBUG_ROW=1)
endif()

set(SHAPONES_PATH ${CMAKE_CURRENT_LIST_DIR}/../../core)
set(XIAO_PATH ${CMAKE_CURRENT_LIST_DIR}/../xiao)

file(GLOB SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/*.c
    ${XIAO_PATH}/common/src/fatfs/*.c
    ${SHAPONES_PATH}/src/*.cpp
)
target_sources(${APP_NAME} PRIVATE ${SRC_FILES}
    # unchanged helpers from the XIAO RP2350 port
    ${XIAO_PATH}/rp/src/lock.cpp
    ${XIAO_PATH}/rp/src/ram.cpp
    ${XIAO_PATH}/rp/src/timer.cpp
    ${XIAO_PATH}/rp/src/gpio.cpp
)
target_compile_options(${APP_NAME} PRIVATE -O2)

target_include_directories(${APP_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${XIAO_PATH}/common/src/fatfs
    ${SHAPONES_PATH}/include
    ${XIAO_PATH}/common/include
)

target_link_libraries(${APP_NAME} PRIVATE
    pico_stdlib pico_multicore
    hardware_spi hardware_i2c hardware_pwm hardware_dma hardware_irq
    hardware_sync hardware_watchdog hardware_clocks
)

if (NES_BT)
    include(${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-extras/bluetooth/bluetooth.cmake)
    target_sources(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bt/bt.cpp)
    target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bt)
    # the radio's SPI runs off the system clock, which is overclocked to 250 MHz
    # here: divided by 3, it stays under the radio chip's 50 MHz
    target_compile_definitions(${APP_NAME} PRIVATE NES_BT=1 CYW43_PIO_CLOCK_DIV_INT=3)
    target_link_libraries(${APP_NAME} PRIVATE picocalc_btkbd)
endif()

pico_set_program_name(${APP_NAME} "ShapoNES for PicoCalc")
pico_enable_stdio_usb(${APP_NAME} 0)
pico_enable_stdio_uart(${APP_NAME} 0)
pico_add_extra_outputs(${APP_NAME})
