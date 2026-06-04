cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_STANDARD 11)

project(LETMEDRAWINPC C)

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

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED IMPORTED_TARGET gtk4>=4.14)
target_link_libraries(LETMEDRAWINPC 
        PRIVATE PkgConfig::GTK
        ws2_32)