cmake_minimum_required(VERSION 3.31)
project(LETMEDRAWINPC C)

set(CMAKE_C_STANDARD 11)


add_executable(LETMEDRAWINPC
        main.c
        Client.c
        Client.h
        Debug.h
        Debug.c
        Controls.c
        Controls.h
        Server.c
        Server.h

)
target_link_libraries(LETMEDRAWINPC ws2_32)

include_directories(C:/msys2/ucrt64/include/gtk-4.0)



