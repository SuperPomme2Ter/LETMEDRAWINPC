#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#include "Server.h"
#include <windows.h>


void CreateInputs(INPUT*(* a)[12][2],const int* inputSize) {

    for(int i=0;i<12;i++) {

        for(int j=0;j<2;j++) {

            //ZeroMemory(*a[i][j],sizeof(INPUT)*inputSize[i]);

            (*a)[i][j]=calloc((size_t)inputSize[i], sizeof(INPUT));
            //memset((*a)[i][j],0,sizeof(INPUT)*inputSize[i]);


            //ZeroMemory(*a[i][j],sizeof(*a[i][j]));
            if((*a)[i][j]==NULL) {

                for(int k=0;k<i;k++) {
                    for(int l=0;l<2;l++) {
                        if (k == i && l >= j) break;
                        free((*a)[k][l]);
                    }
                }
                printf("Memory allocation failed\n");
                exit(1);
            }
            for (int k = 0; k < inputSize[i]; k++) {
                (*a)[i][j][k].type = 0;
            }
        }
    }


    // if (isKeyUp) {
    //     *a[index][1]=malloc(sizeof(INPUT)*size);
    //     if (*a[index][1]==NULL) {
    //         printf("Memory allocation failed\n");
    //         exit(1);
    //     }
    // }
}
void CreateInputKeyboard(INPUT*(*inputs)[2] , int size, ...) {
    va_list args;
    va_start(args, size);

    for (int idx = 0; idx < size; idx++) {
        int vk = va_arg(args, int);


        // inputs[0][idx]->type = INPUT_KEYBOARD;
        // inputs[0][idx]->ki.wVk=va_arg(args, int);
        // inputs[1][idx]->type = INPUT_KEYBOARD;
        // inputs[1][idx]->ki.wVk=va_arg(args, int);
        // inputs[1][idx]->ki.dwFlags=KEYEVENTF_KEYUP;

        (*inputs)[0][idx].type = INPUT_KEYBOARD;
        (*inputs)[0][idx].ki.wVk = (WORD)vk;
        (*inputs)[0][idx].ki.dwFlags = 0;

        (*inputs)[1][idx].type = INPUT_KEYBOARD;
        (*inputs)[1][idx].ki.wVk = (WORD)vk;
        (*inputs)[1][idx].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    va_end(args);
}
int CreateInputMouse(INPUT*(*inputs)[2], int size, ...) {
    if (size%2 != 0) {
        printf("Size can only be a multiple of 2 : \n one for input down \n one for input up");
        return -1;
    }
    va_list args;
    va_start(args, size);

    // for (int i=1; i<size; i++) {
    //
    //     inputs[0][i]->type = INPUT_MOUSE;
    //     inputs[0][i]->mi.dwFlags=va_arg(args, int);
    //     inputs[1][i]->type = INPUT_MOUSE;
    //     inputs[1][i]->mi.dwFlags=va_arg(args, int);
    // }
    for (int idx = 0; idx < size; idx+=2) {
        int flag_down = va_arg(args, int);
        int flag_up = va_arg(args, int);

        (*inputs)[0][idx].type = INPUT_MOUSE;
        (*inputs)[0][idx].mi.dwFlags = (DWORD)flag_down;

        (*inputs)[1][idx].type = INPUT_MOUSE;
        (*inputs)[1][idx].mi.dwFlags = (DWORD)flag_up;
    }

    va_end(args);
    return 0;


}


// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH

int main(void) {
    INPUT *inputs[12][2];
    memset(&inputs, 0, sizeof(inputs));
    const int inputSize[12] = {
        1,
        2,
        1,
        1,
        2,
        2,
        1,
        1,
        1,
        1,
        1,
        1
    };

    // #define ABUTTON 0
    // #define BBUTTON 1
    // #define XBUTTON 2
    // #define YBUTTON 3
    // #define LBUTTON 4
    // #define RBUTTON 5
    // #define UPBUTTON 6
    // #define DOWNBUTTON 7
    // #define LEFTBUTTON 8
    // #define RIGHTBUTTON 9
    // #define SELECTBUTTON 10
    // #define TOUCHSCREEN 11


    CreateInputs(&inputs, inputSize);
    CreateInputKeyboard(&inputs[ABUTTON], inputSize[0],VK_SPACE);
    CreateInputKeyboard(&inputs[BBUTTON], inputSize[1],VK_CONTROL, 'Z');
    CreateInputKeyboard(&inputs[XBUTTON], inputSize[2],VK_SHIFT);
    CreateInputKeyboard(&inputs[YBUTTON], inputSize[3],VK_CONTROL);

    if (CreateInputMouse(&inputs[LBUTTON], inputSize[4],MOUSEEVENTF_LEFTDOWN,MOUSEEVENTF_LEFTUP) < 0) {
        printf("CreateInputMouse failed\n");

        for (int i = 0; i < 4; i++) {
            const unsigned int a = SendInput(inputSize[i], inputs[i][1], sizeof(INPUT));
            if (a != inputSize[i]) {
                printf("input error\n");
            }
            printf("releasing input %d\n", i);
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 2; j++) {
                printf("input freed %d\n", i);
                free(inputs[i][j]);
            }
        }
    }

    if (CreateInputMouse(&inputs[RBUTTON], inputSize[5],MOUSEEVENTF_RIGHTDOWN,MOUSEEVENTF_RIGHTUP) < 0) {
        printf("CreateInputMouse failed\n");

        for (int i = 0; i < 5; i++) {
            const unsigned int a = SendInput(inputSize[i], inputs[i][1], sizeof(INPUT));
            if (a != inputSize[i]) {
                printf("input error\n");
            }
            printf("releasing input %d\n", i);
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 2; j++) {
                printf("input freed %d\n", i);
                free(inputs[i][j]);
            }
        }
    }

    CreateInputKeyboard(&inputs[UPBUTTON], inputSize[6],VK_UP);
    CreateInputKeyboard(&inputs[DOWNBUTTON], inputSize[7],VK_DOWN);
    CreateInputKeyboard(&inputs[LEFTBUTTON], inputSize[8],VK_LEFT);
    CreateInputKeyboard(&inputs[RIGHTBUTTON], inputSize[9],VK_RIGHT);
    CreateInputKeyboard(&inputs[SELECTBUTTON], inputSize[10],VK_RETURN);


    // void CreateInputKeyboard(INPUT*(*inputs)[2] , int size, ...) {
    //     va_list args;
    //     va_start(args, size);
    //
    //     for (int idx = 0; idx < size; idx++) {
    //         int vk = va_arg(args, int);
    //
    //
    //         // inputs[0][idx]->type = INPUT_KEYBOARD;
    //         // inputs[0][idx]->ki.wVk=va_arg(args, int);
    //         // inputs[1][idx]->type = INPUT_KEYBOARD;
    //         // inputs[1][idx]->ki.wVk=va_arg(args, int);
    //         // inputs[1][idx]->ki.dwFlags=KEYEVENTF_KEYUP;
    //
    //         (*inputs)[0][idx].type = INPUT_KEYBOARD;
    //         (*inputs)[0][idx].ki.wVk = (WORD)vk;
    //         (*inputs)[0][idx].ki.dwFlags = 0;
    //
    //         (*inputs)[1][idx].type = INPUT_KEYBOARD;
    //         (*inputs)[1][idx].ki.wVk = (WORD)vk;
    //         (*inputs)[1][idx].ki.dwFlags = KEYEVENTF_KEYUP;
    //     }
    //
    //     va_end(args);
    // }

    inputs[TOUCHSCREEN][0]->type=INPUT_MOUSE;
    inputs[TOUCHSCREEN][0]->mi.dwFlags = MOUSEEVENTF_MOVE;
    inputs[TOUCHSCREEN][0]->mi.dwFlags = MOUSEEVENTF_MOVE;
    // INPUT input;
    // input.type = INPUT_MOUSE;
    inputs[TOUCHSCREEN][0]->mi.mouseData = 0;
    // input.mi.dx = x * (65536 / GetSystemMetrics(SM_CXSCREEN)); //x being coord in pixels
    // input.mi.dy =  y * (65536 / GetSystemMetrics(SM_CYSCREEN)); //y being coord in pixels
    // input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    // SendInput(1, &input, sizeof(input));

    // printf("%d\n",inputs[LBUTTON][0]->mi.dwFlags);
    // printf("%d\n",inputs[LBUTTON][1]->mi.dwFlags);


    // INPUT inputs[1];


    // ZeroMemory(inputs, sizeof(inputs));
    // inputs[0].type = INPUT_MOUSE;
    // inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;


    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 0;
    }


    char host[256];

    struct hostent *host_entry;

    memset(&host, 0, sizeof(host));


    host_entry = gethostbyname(host);
    uint32_t *IP = (uint32_t *) host_entry->h_addr_list[
        0];


    struct sockaddr_in DSAdr;


    memset(&DSAdr, 0, sizeof(DSAdr));


    struct addrinfo *PCAdrInfo;


    if (!ClientStart()) {
        printf("Closing socket\n");
        WSACleanup();

        return (0);
    }
    ServerPart(IP, &inputs, inputSize);
    WSACleanup();


    for (int i = 0; i < 11; i++) {
        const unsigned int a = SendInput(inputSize[i], inputs[i][1], sizeof(INPUT));
        if (a != inputSize[i]) {
            printf("input error\n");
        }
        printf("releasing input %d\n", i);
    }
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 2; j++) {
            printf("input freed %d\n", i);
            free(inputs[i][j]);
        }
    }
    return 0;
}
