#include "Controls.h"

#include <stdio.h>

int GetFLag(uint16_t flags, int index) {

    flags = flags >>index;
    return flags & 1;

}

int GenerateInput(INPUT* inputToSend, int size) {

    const unsigned int a=SendInput(size, inputToSend, sizeof(INPUT));
    if (a!=size) {
        printf("Input error\n");
        return -1;
    }
    return 0;

}

int GenerateMouseMvmtInput(INPUT* inputToSend,int coordX, int coordY) {
    inputToSend[0].mi.dx=coordX*(65536 / GetSystemMetrics(SM_CXSCREEN));
    inputToSend[0].mi.dy=coordY*(65536 / GetSystemMetrics(SM_CYSCREEN));
    if (GenerateInput(inputToSend,1)<0) {
        return -1;
    }
    return 0;

}




void ReadFlags(uint16_t flags,uint16_t lastFlagsValue,INPUT*(*inputToSend)[12][2],int inputsSize[12]) {

    for (int i=0;i<11;i++) {
        if (GetFLag(flags,i)) {
            if (GenerateInput((*inputToSend)[i][0],inputsSize[i])<0) {
                printf("Generate input error\n");
            }
        }else if (GetFLag(lastFlagsValue,i)) {
            if (GenerateInput((*inputToSend)[i][1],inputsSize[i])<0) {
                printf("Generate input error\n");
            }
        }
    }
}


short GetBitShiftingOccurence16Bits(uint16_t bitmask) {
    short occurence=1;
    for (short i = 0; i < 16; i++) {
        if (bitmask & 1) {
            return occurence;
        }
        bitmask >>=1;
        occurence ++;

    }
    return -1;
}