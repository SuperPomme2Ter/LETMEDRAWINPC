#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Client.h"
#include "Server.h"
#include <windows.h>

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

#define ARRSIZE(x)  (sizeof(x) / sizeof((x)[0]))


void bindInput(INPUT* f,int VK[],int size,int isKeyUp, int isKeyboard) {

    for (int i = 0; i<size; i++) {
        if (isKeyboard) {
            f[i].type=INPUT_KEYBOARD;
            f[i].ki.wVk = VK[i];
            if (isKeyUp) {
                f[i].ki.dwFlags = KEYEVENTF_KEYUP;
            }
            f[i].ki.time = 0;
            return;
        }
        f[i].type = INPUT_MOUSE;
        f[i].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;




    }
}


// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH

int main(void) {
    // INPUT* inputs[12][2];
    //
    //
    // INPUT inputAON[1];
    // ZeroMemory(inputAON, sizeof(inputAON));
    // {
    //     int t[1]={VK_SPACE};
    //     bindInput(&inputAON,t,1,0,1);
    // }
    // inputs[0][0]=&inputAON;
    //
    // INPUT inputAOFF[1];
    // ZeroMemory(inputAOFF, sizeof(inputAOFF));
    // {
    //     int t[1]={VK_SPACE};
    //     bindInput(&inputAOFF,t,1,1,1);
    // }
    // inputs[0][1]=&inputAOFF;
    //
    // INPUT inputBON[2];
    // ZeroMemory(inputBON, sizeof(inputBON));
    // {
    //     int t[2]={VK_CONTROL,0x5A /*Z (POURQUOI IL N'Y A PAS DE MACRO POUR LES CHIFFRES ET LES LETTRES ???)*/};
    //     bindInput(&inputBON,t,2,0,1);
    // }
    // inputs[1][0]=&inputBON;
    //
    // INPUT inputBOFF[2];
    // ZeroMemory(inputBOFF, sizeof(inputBOFF));
    // {
    //     int t[2]={VK_CONTROL,0x5A /* Z */};
    //     bindInput(&inputBOFF,t,2,1,1);
    // }
    // inputs[1][1]=&inputBOFF;
    //
    // INPUT inputXON[1];
    // ZeroMemory(inputXON, sizeof(inputXON));
    // {
    //     int t[1]={VK_SHIFT};
    //     bindInput(&inputXON,t,1,0,1);
    // }
    // inputs[2][0]=&inputXON;
    //
    // INPUT inputXOFF[1];
    // ZeroMemory(inputXOFF, sizeof(inputXOFF));
    // {
    //     int t[1]={VK_SHIFT};
    //     bindInput(&inputXOFF,t,1,1,1);
    // }
    // inputs[2][1]=&inputXOFF;
    //
    // INPUT inputYON[1];
    // ZeroMemory(inputYON, sizeof(inputYON));
    // {
    //     int t[1]={VK_CONTROL};
    //     bindInput(&inputYON,t,1,0,1);
    // }
    // inputs[3][0]=&inputYON;
    //
    // INPUT inputYOFF[1];
    // ZeroMemory(inputYOFF, sizeof(inputYOFF));
    // {
    //     int t[1]={VK_CONTROL};
    //     bindInput(&inputYOFF,t,1,1,1);
    // }
    // inputs[3][1]=&inputYOFF;
    //
    // INPUT inputLON[1];
    // ZeroMemory(inputLON, sizeof(inputLON));
    // {
    //     int t[1]={MOUSEEVENTF_LEFTDOWN};
    //     bindInput(&inputLON,t,1,0,0);
    // }

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

     host_entry = gethostbyname(host);
     uint32_t  *IP= (uint32_t *) host_entry->h_addr_list[0];


     struct sockaddr_in DSAdr;

     memset(&DSAdr, 0, sizeof(DSAdr));



     struct addrinfo *PCAdrInfo;


     if (!ClientStart())
     {
         printf("Closing socket\n");
         WSACleanup();
         return (0);
     }
     ServerPart(IP);
     WSACleanup();



     return 0;

 }

