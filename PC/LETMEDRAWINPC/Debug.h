//
// Created by rapha on 14/07/2025.
//
#pragma once

#include <stdint.h>
#include <time.h>
#include <winsock2.h>

void Print_wsa_error(const char *msg);

void GetCPUClockTimeTaken(clock_t* t, uint8_t refreshTime);
