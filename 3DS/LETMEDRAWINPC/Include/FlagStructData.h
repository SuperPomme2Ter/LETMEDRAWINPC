//
// Created by SuperPomme2Ter on 21/09/2025.
//

#pragma once

#include <sys/types.h>
#include <3ds/types.h>
#include <3ds/services/hid.h>

#ifndef LETMEDRAWINPC_FLAGSTRUCTDATA_H
#define LETMEDRAWINPC_FLAGSTRUCTDATA_H



#define ABUTTON BIT(1)
#define BBUTTON BIT(2)
#define XBUTTON BIT(3)
#define YBUTTON BIT(4)
#define LBUTTON BIT(5)
#define RBUTTON BIT(6)
#define UPBUTTON BIT(7)
#define DOWNBUTTON BIT(8)
#define LEFTBUTTON BIT(9)
#define RIGHTBUTTON BIT(10)
#define SELECTBUTTON BIT(11)
#define TOUCHSCREEN BIT(12)



#endif //LETMEDRAWINPC_FLAGSTRUCTDATA_H



/*  MASK LISIBLE
  0b 0000 0000 0001 0000 0000 1111 1111 0111 */
extern const u_int32_t mask3DSToPC;
u_int16_t ConvertToAppFlag(u_int32_t mask3DS);

