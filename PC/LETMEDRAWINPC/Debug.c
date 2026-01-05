//
// Created by rapha on 22/07/2025.
//
// shared.c
#include "Debug.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

void Print_wsa_error(const char *msg) {
    DWORD err = WSAGetLastError();
    char *s = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPSTR)&s, 0, NULL);
    fprintf(stderr, "%s: %s\n", msg, s);
    LocalFree(s);


}
void GetCPUClockTimeTaken(clock_t* t, uint8_t refreshTime) {

    clock_t tmp=clock()-*t;
    if (refreshTime) {
        *t=clock();
    }
    if (tmp>5) {
        printf("number of Clock cycle since last refresh : %ld (%f seconds)\n\n",tmp, ((float)tmp) / CLOCKS_PER_SEC);
    }

}