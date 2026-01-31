//
// Created by rapha on 23/07/2025.
//
#include <stdio.h>
#include <ws2tcpip.h>

#include "Server.h"

#include <conio.h>
#include <stdint.h>



#define BACKLOG 10
#define SERVPORT "8000"

#define NOTOUCH 999
#define XTOUCH 320
#define YTOUCH 240


// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH



void CloseAll(int* SocketDS, int* SocketPC) {

    printf("Closing client socket\n");
    if (SocketDS != 0 && *SocketDS != -1) {
        printf("Closing client socket\n");
        closesocket(*SocketDS);
        *SocketDS = -1;
    }



    printf("Closing server socket\n");
    if (SocketPC != 0 && *SocketPC != -1) {
        printf("Closing server socket\n");
        closesocket(*SocketPC);
        *SocketPC = -1;
    }

}

__attribute__ ((hot))
int IsInProgressOrBlockingStatus() {
    int err = WSAGetLastError();
    if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS) {

        return 1;
    }
    return 0;

}

__attribute__ ((hot))
int ReadDSScreenCoordinates(const uint16_t coordinatesRecv[3],short actualCoordinates[2])
{
        if (coordinatesRecv[1]==NOTOUCH
            ||  coordinatesRecv[1]<=0
            ||  coordinatesRecv[1]>=XTOUCH
            ||  coordinatesRecv[2]<=0
            ||  coordinatesRecv[2]>=YTOUCH) { return 0; }

        actualCoordinates[0] = (short)-((XTOUCH*0.5)- coordinatesRecv[1]);
        actualCoordinates[1] = (short)-((YTOUCH*0.5)- coordinatesRecv[2]);

        return 1;
}

__attribute__ ((hot))
int ReadDSFlags(const uint16_t* flagsRecv,uint16_t* actualFlags) {


    if (*flagsRecv==*actualFlags || *flagsRecv>=SHRT_MAX) {

        return 0;
    }
    *actualFlags= *flagsRecv;


    return 1;

}

__attribute__ ((hot))
int ReadDSInputInfo (int *DSSocket,int *PCSocket,uint16_t* flagsBuffer,short coordinatesBuffer[2]) {



    uint16_t buffer[3];
    int bytes_read=0;

    memset(buffer, 0, sizeof buffer);

    bytes_read = recv(*DSSocket, buffer, sizeof(buffer), 0);


    if (bytes_read<0){
        if (IsInProgressOrBlockingStatus()) {
            return 0;
        }
        Print_wsa_error("socket fd error");
        printf("AAAAAAAAAAAAA");
        CloseAll(DSSocket,PCSocket);
        return -1;
    }
    if (bytes_read == 0) {
        printf("[%d] Client socket closed connection.\n", *DSSocket);
        Print_wsa_error("socket fd error");
        CloseAll(DSSocket,PCSocket);
        return -1;
    }
    if (bytes_read != sizeof(buffer)) {
        printf("not everything was recv\n");
        return 0;
    }
    //GetNumberOfLoopTimeTaken(t,1);
    ReadDSFlags(&buffer[0], flagsBuffer);



    if (!ReadDSScreenCoordinates(&buffer, coordinatesBuffer)) {
        coordinatesBuffer[0]= NOTOUCH;
        coordinatesBuffer[1]= NOTOUCH;

        return 1;
    }
    return 1;

}


int ServerPart(const uint32_t *PCIP, INPUT*(* inputs)[11][2], int inputSize[12]) {

    printf("Entering server mode\n\n");
    short posBuffer[2]= {0,0};
    uint16_t flags=0;
    int status;

    u_long nonblock = 1;

    struct sockaddr_in PCAdress;

    int SocketDS=-1;

    struct sockaddr_in DSAdr;

    memset(&PCAdress, 0, sizeof (PCAdress));


    /*SocketDS=socket(AF_INET, SOCK_STREAM, 0);

    if (SocketDS == SOCKET_ERROR) {
        Print_wsa_error("socket fd error");
        closesocket(SocketDS);
        return (1);
    }*/

    int SocketPC = socket(AF_INET, SOCK_STREAM, 0);

    if (SocketPC == SOCKET_ERROR) {
        Print_wsa_error("socket fd error");
        closesocket(SocketPC);
        return (1);
    }

    printf("Created server socket fd: %d\n", SocketPC);

    PCAdress.sin_family = AF_INET;
    PCAdress.sin_addr.s_addr = *PCIP;
    PCAdress.sin_port = htons(4242);



    status = bind(SocketPC, (struct sockaddr *) &PCAdress, sizeof(PCAdress));
    if (status != 0) {
        Print_wsa_error("socket fd error");
        closesocket(SocketPC);
        return (2);
    }
    printf("Bound socket to localhost port %d\n", PCAdress.sin_port);


    printf("Listening on port %d\n", PCAdress.sin_port);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {

        Print_wsa_error("socket fd error");
        closesocket(SocketPC);
        return (3);
    }
    printf("[Server] Set up select fd sets\n");
    ioctlsocket(SocketPC, FIONBIO, &nonblock);


    int lastCursorPos[2]= {0,0};
    int absoluteCursorPos[2];

    //GetCursorPos(absoluteCursorPos);

    int DSConnected = 0;

    INT32 DSAdrLength = sizeof (DSAdr);
    uint16_t lastFlagsValue=flags;

    while (!kbhit())
    {

        if (!DSConnected) {

            SocketDS = accept(SocketPC, (struct sockaddr *) &DSAdr, &DSAdrLength);

            if (!IsInProgressOrBlockingStatus()) {
                if (SocketDS < 0) {
                    Print_wsa_error("socket fd error");
                    CloseAll(&SocketDS, &SocketPC);
                    return (1);
                }
                DSConnected = 1;
                printf("You can now use your 3DS as a controller\n");
                ioctlsocket(SocketDS, FIONBIO, &nonblock);

            }

        }
        else {
            status=ReadDSInputInfo(&SocketDS, &SocketPC, &flags, posBuffer);

            if (status==1) {

                if (GetFLag(flags,TOUCHSCREEN) && posBuffer[0]!=NOTOUCH) {

                    absoluteCursorPos[0] = posBuffer[0]+lastCursorPos[0];
                    absoluteCursorPos[1] = posBuffer[1]+lastCursorPos[1];

                    // if (!SetCursorPos(absoluteCursorPos[0], absoluteCursorPos[1])) {
                    //     printf("set cursor fail\n");
                    //
                    // }
                    if (GenerateMouseMvmtInput((*inputs)[11][0],posBuffer[0],posBuffer[1])<0) {
                        printf("cursor movement failed\n");
                    }
                    // else {
                    //     printf("Nope\n");
                    // }
                }
                else if (GetFLag(lastFlagsValue,TOUCHSCREEN)) {

                    lastCursorPos[0] = absoluteCursorPos[0];
                    lastCursorPos[1] = absoluteCursorPos[1];
                }

                if (flags != lastFlagsValue) {
                    ReadFlags(flags,lastFlagsValue, inputs,inputSize);
                    lastFlagsValue = flags;
                }
            }
            else if (status<0) {
                break;
            }
        }
    }

    CloseAll(&SocketDS, &SocketPC);
    return 0;
}