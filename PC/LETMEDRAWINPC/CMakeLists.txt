cmake_minimum_required(VERSION 3.31)
project(LETMEDRAWINPC C)

set(CMAKE_C_STANDARD 11)

add_executable(LETMEDRAWINPC
        main.c
test.c test.h)
target_link_libraries(LETMEDRAWINPC ws2_32)
#[[
include_directories(F:/devkitPro/devkitARM/include)
include_directories(F:/devkitPro/libctru/include)
include_directories(F:/devkitPro/devkitARM/arm-none-eabi/include)
]]

