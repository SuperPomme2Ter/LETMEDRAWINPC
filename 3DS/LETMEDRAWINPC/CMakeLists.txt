cmake_minimum_required(VERSION 3.31)
project(LETMEDRAWINPC C)

set(CMAKE_C_STANDARD 11)

add_executable(LETMEDRAWINPC
        Source/main.c
        Source/FlagStructData.c
        Include/FlagStructData.h

)
include_directories(Include)
include_directories(C:/devkitPro/devkitARM/include)
include_directories(C:/devkitPro/libctru/include)
include_directories(C:/devkitPro/devkitARM/arm-none-eabi/include)
