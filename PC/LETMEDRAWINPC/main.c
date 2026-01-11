#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#include "Server.h"
#include <windows.h>




void bindInput(INPUT f[],int VK[], const int size, const int isKeyUp, const int isKeyboard) {

    for (int i = 0; i<size; i++) {
        if (isKeyboard) {
            f[i].type=INPUT_KEYBOARD;
            f[i].ki.wVk = VK[i];
            if (isKeyUp) {
                f[i].ki.dwFlags = KEYEVENTF_KEYUP;
            }
            f[i].ki.time = 0;
            return;
        }else {
            f[i].type = INPUT_MOUSE;
        }


    }
}


// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH

int main(void) {


    INPUT* inputs[11][2];

    INPUT inputAON[1];
    //ZeroMemory(inputAON, sizeof(inputAON));
    {
        int t[1]={VK_SPACE};
        bindInput(inputAON,t,1,0,1);
    }
    inputs[0][0]=&inputAON;

    INPUT inputAOFF[1];
    ///ZeroMemory(inputAOFF, sizeof(inputAOFF));
    {
        int t[1]={VK_SPACE};
        bindInput(inputAOFF,t,1,1,1);
    }
    inputs[0][1]=&inputAOFF;

    INPUT inputBON[2];
    //ZeroMemory(inputBON, sizeof(inputBON));
    {
        int t[2]={VK_CONTROL,0x5A /*Z (POURQUOI IL N'Y A PAS DE MACRO POUR LES CHIFFRES ET LES LETTRES ???)*/};
        bindInput(inputBON,t,2,0,1);
    }
    inputs[1][0]=&inputBON;

    INPUT inputBOFF[2];
    //ZeroMemory(inputBOFF, sizeof(inputBOFF));
    {
        int t[2]={VK_CONTROL,0x5A /* Z */};
        bindInput(inputBOFF,t,2,1,1);
    }
    inputs[1][1]=&inputBOFF;

    INPUT inputXON[1];
    //ZeroMemory(inputXON, sizeof(inputXON));
    {
        int t[1]={VK_SHIFT};
        bindInput(inputXON,t,1,0,1);
    }
    inputs[2][0]=&inputXON;

    INPUT inputXOFF[1];
    //ZeroMemory(inputXOFF, sizeof(inputXOFF));
    {
        int t[1]={VK_SHIFT};
        bindInput(inputXOFF,t,1,1,1);
    }
    inputs[2][1]=&inputXOFF;

    INPUT inputYON[1];
    //ZeroMemory(inputYON, sizeof(inputYON));
    {
        int t[1]={VK_CONTROL};
        bindInput(inputYON,t,1,0,1);
    }
    inputs[3][0]=&inputYON;

    INPUT inputYOFF[1];
    //ZeroMemory(inputYOFF, sizeof(inputYOFF));
    {
        int t[1]={VK_CONTROL};
        bindInput(inputYOFF,t,1,1,1);
    }
    inputs[3][1]=&inputYOFF;

    INPUT inputLON[1];
    //ZeroMemory(inputLON, sizeof(inputLON));
    {
        int t[1]={MOUSEEVENTF_LEFTDOWN};
        bindInput(inputLON,t,1,0,0);
    }
    inputs[4][0]=&inputLON;


    INPUT inputLOFF[1];
    //ZeroMemory(inputLOFF, sizeof(inputLOFF));
    {
        int t[1]={MOUSEEVENTF_LEFTDOWN};
        bindInput(inputLOFF,t,1,1,0);
    }
    inputs[4][1]=&inputLOFF;

    INPUT inputRON[1];
    //ZeroMemory(inputRON, sizeof(inputRON));
    {
        int t[1]={MOUSEEVENTF_RIGHTDOWN};
        bindInput(inputRON,t,1,0,0);
    }
    inputs[5][0]=&inputRON;

    INPUT inputROFF[1];
    //ZeroMemory(inputROFF,sizeof(inputROFF));
    {
        int t[1]={MOUSEEVENTF_RIGHTUP};
        bindInput(inputROFF,t,1,1,0);
    }
    inputs[5][1]=&inputROFF;

    INPUT inputUpON[1];
    //ZeroMemory(inputUpON, sizeof(inputUpON));
    {
        int t[1]={VK_UP};
        bindInput(inputUpON,t,1,0,1);
    }
    inputs[6][0]=&inputUpON;

    INPUT inputUpOFF[1];
    //ZeroMemory(inputUpOFF, sizeof(inputUpOFF));
    {
        int t[1]={VK_UP};
        bindInput(inputUpOFF,t,1,1,1);
    }
    inputs[6][1]=&inputUpOFF;


    INPUT inputDownON[1];
    //ZeroMemory(inputDownON, sizeof(inputDownON));
    {
        int t[1]={VK_DOWN};
        bindInput(inputDownON,t,1,0,1);
    }
    inputs[7][0]=&inputDownON;

    INPUT inputDownOFF[1];
    //ZeroMemory(inputDownOFF, sizeof(inputDownOFF));
    {
        int t[1]={VK_DOWN};
        bindInput(inputDownOFF,t,1,1,1);
    }
    inputs[7][1]=&inputDownOFF;

    INPUT inputLeftON[1];
    //ZeroMemory(inputLeftON, sizeof(inputLeftON));
    {
        int t[1]={VK_LEFT};
        bindInput(inputLeftON,t,1,0,1);
    }
    inputs[8][0]=&inputLeftON;

    INPUT inputLeftOFF[1];
    //ZeroMemory(inputLeftOFF, sizeof(inputLeftOFF));
    {
        int t[1]={VK_LEFT};
        bindInput(inputLeftOFF,t,1,1,1);
    }
    inputs[8][1]=&inputLeftOFF;

    INPUT inputRightON[1];
    //ZeroMemory(inputRightON, sizeof(inputRightON));
    {
        int t[1]={VK_RIGHT};
        bindInput(inputRightON,t,1,0,1);
    }
    inputs[9][0]=&inputRightON;

    INPUT inputRightOFF[1];
    //ZeroMemory(inputRightOFF, sizeof(inputRightOFF));
    {
        int t[1]={VK_RIGHT};
        bindInput(inputRightOFF,t,1,1,1);
    }
    inputs[9][1]=&inputRightOFF;

    INPUT inputSelectON[1];
    //ZeroMemory(inputSelectON, sizeof(inputSelectON));
    {
        int t[1]={VK_RETURN};
        bindInput(inputSelectON,t,1,0,1);
    }
    inputs[10][0]=&inputSelectON;

    INPUT inputSelectOFF[1];
    //ZeroMemory(inputSelectOFF, sizeof(inputSelectOFF));
    {
        int t[1]={VK_RETURN};
        bindInput(inputSelectOFF,t,1,1,1);
    }
    inputs[10][1]=&inputSelectOFF;


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

     struct hostent* host_entry;

     memset(&host,0,sizeof(host));

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
     ServerPart(IP,inputs);
     WSACleanup();



     return 0;

 }

