//
// Created by rapha on 29/07/2025.
//

#pragma once
#include <stdint.h>

#include "Debug.h"

#define ABUTTON 0
#define BBUTTON 1
#define XBUTTON 2
#define YBUTTON 3
#define LBUTTON 4
#define RBUTTON 5
#define UPBUTTON 6
#define DOWNBUTTON 7
#define LEFTBUTTON 8
#define RIGHTBUTTON 9
#define SELECTBUTTON 10
#define TOUCHSCREEN 11

int GenerateInput(INPUT* inputToSend,int size);

int GetFLag(uint16_t flags, int index);

void ReadFlags(uint16_t flags,uint16_t lastFlagsValue,INPUT*(*inputToSend)[11][2], int inputsSize[11]);

short GetBitShiftingOccurence16Bits(uint16_t bitmask);



