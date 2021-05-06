# cmake_minimum_required(VERSION <specify CMake version here>)
project(ylwm C)
set(CMAKE_C_STANDARD 99)

add_executable(ylwm main.c)
TARGET_LINK_LIBRARIES(ylwm xcb xkbcommon xcb-util)
#target_link_libraries(ylwm /usr/lib/libxcb.so)

