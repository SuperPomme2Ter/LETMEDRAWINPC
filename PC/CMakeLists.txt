cmake_minimum_required(VERSION 3.31)
project(PC C)

set(CMAKE_C_STANDARD 11)

add_executable(PC
        test.c
        test.h
        main.c)

include_directories(F:/devkitPro/devkitARM/include)
include_directories(F:/devkitPro/libctru/include)
include_directories(F:/devkitPro/devkitARM/arm-none-eabi/include)