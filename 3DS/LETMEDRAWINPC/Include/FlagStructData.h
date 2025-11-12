//
// Created by SuperPomme2Ter on 21/09/2025.
//

#pragma once

#include <sys/types.h>
#include <3ds/types.h>
#include <3ds/services/hid.h>

#ifndef LETMEDRAWINPC_FLAGSTRUCTDATA_H
#define LETMEDRAWINPC_FLAGSTRUCTDATA_H



#define ABUTTON BIT(0)
#define BBUTTON BIT(1)
#define SELECTBUTTON BIT(2)
#define LEFTBUTTON BIT(3)
#define RIGHTBUTTON BIT(4)
#define UPBUTTON BIT(5)
#define DOWNBUTTON BIT(6)
#define RBUTTON BIT(7)
#define LBUTTON BIT(8)
#define XBUTTON BIT(9)
#define YBUTTON BIT(10)
#define TOUCHSCREEN BIT(11)

/*  MASK LISIBLE
  0b 0000 0000 0001 0000 0000 1111 1111 0111 */
extern const u_int32_t mask3DSToPC;
u_int16_t ConvertToAppFlag(u_int32_t mask3DS);


#endif //LETMEDRAWINPC_FLAGSTRUCTDATA_H





