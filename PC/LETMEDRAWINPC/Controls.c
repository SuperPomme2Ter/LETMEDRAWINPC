#include "Controls.h"

#include <stdio.h>

int GetFLag(uint16_t flags, int index) {

    flags >>= index;
    return flags & 1;

}

int GenerateInput(INPUT inputToSend[]) {

    const unsigned int a=SendInput(ARRAYSIZE(inputToSend), &inputToSend, sizeof(INPUT));
    if (a!=ARRAYSIZE(inputToSend)) {
        printf("Input error");
        return -1;
    }
    return 0;

}




void ReadFlags(uint16_t flags,uint16_t lastFlagsValue,INPUT* inputToSend[11][2]) {

    for (int i=0;i<11;i++) {
        if (GetFLag(flags,i)) {
            if (GenerateInput(inputToSend[i][0])<0) {
                printf("Generate input error");
            }
        }else if (GetFLag(lastFlagsValue,i)) {
            if (GenerateInput(inputToSend[i][1])<0) {
                printf("Generate input error");
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