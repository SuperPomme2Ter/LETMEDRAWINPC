//
// Created by rapha on 29/07/2025.
//

#pragma once
#include <stdint.h>

#include "Debug.h"

#define ABUTTON 0
#define BBUTTON 1
#define SELECTBUTTON 2
#define RIGHTBUTTON 3
#define LEFTBUTTON 4
#define UPBUTTON 5
#define DOWNBUTTON 6
#define RBUTTON 7
#define LBUTTON 8
#define XBUTTON 9
#define YBUTTON 10
#define TOUCHSCREEN 11


int GenerateInput(INPUT* inputToSend,int size);

int GenerateMouseMvmtInput(INPUT* inputToSend,int coordX, int coordY);

int GetFLag(uint16_t flags, int index);

void ReadFlags(uint16_t flags,uint16_t lastFlagsValue,INPUT*(*inputToSend)[12][2], int inputsSize[12]);

short GetBitShiftingOccurence16Bits(uint16_t bitmask);



