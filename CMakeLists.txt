CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

INCLUDE(pico_sdk_import.cmake)

PROJECT(picker)

pico_sdk_init()

add_executable(app main.cpp display.cpp state.cpp gui.cpp core.cpp classifier.cpp ov7670.cpp picker.cpp arm_movement.cpp sg90.cpp input.cpp)

target_link_libraries(app PRIVATE pico_stdlib hardware_spi pico_multicore hardware_adc hardware_dma hardware_irq hardware_gpio hardware_i2c hardware_gpio hardware_pwm hardware_flash)

pico_add_extra_outputs(app)

add_subdirectory(edge)

pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 0)