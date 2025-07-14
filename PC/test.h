//
// Created by rapha on 14/07/2025.
//

#ifndef TEST_H
#define TEST_H

#endif //TEST_H
#pragma once

#ifndef WINVER
    #define WINVER 0x0500
#endif

#include <windows.h>
#include <winsock.h>

#include <stddef.h>

#define SCREENSHOT_CHUNK 4000

#define IP INADDR_ANY

extern SOCKET listener;
extern SOCKET client;