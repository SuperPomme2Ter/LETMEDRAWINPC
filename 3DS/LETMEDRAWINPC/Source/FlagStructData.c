//
// Created by SuperPomme2Ter on 21/09/2025.
//

#pragma once

#include "FlagStructData.h"



const u_int32_t mask3DSToPC = 0b00000000000100000000111111110111;

__attribute__ ((hot))
u_int16_t ConvertToAppFlag(u_int32_t flag3DS) {


    int n = 0;
    u_int16_t rslt=0;
    for (int i = 0; i<32; i++){

        if (((mask3DSToPC >> i) & 1) == 1){
            rslt |= ((BIT(i) & flag3DS) >> (i-n));
            n++;
        }
    }
    return rslt;
}