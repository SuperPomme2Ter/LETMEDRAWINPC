//
// Created by rapha on 22/07/2025.
//
// shared.c
#include "Debug.h"

#include <assert.h>
#include <stdio.h>
#include <windows.h>

void print_wsa_error(const char *msg) {
    DWORD err = WSAGetLastError();
    char *s = NULL;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPSTR)&s, 0, NULL);
    fprintf(stderr, "%s: %s\n", msg, s);
    LocalFree(s);


}