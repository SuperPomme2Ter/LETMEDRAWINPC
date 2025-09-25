//
// Created by rapha on 23/07/2025.
//
#include <stdio.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Server.h"

#include <conio.h>
#include <stdint.h>

#include "Controls.h"

#define BACKLOG 10
#define SERVPORT "8000"

#define NOTOUCH 999
#define XTOUCH 320
#define YTOUCH 240


// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH



void CloseAll(int SocketDS, int SocketPC) {

    printf("Closing client socket\n");
    if (SocketDS != 0) closesocket(SocketDS);


    printf("Closing server socket\n");
    if (SocketPC != 0) closesocket(SocketPC);
}
__attribute__ ((hot))
int ReadDSScreenCoordinates(uint16_t* coordinatesRecv,short actualCoordinates[])
{
        if (coordinatesRecv[1]==NOTOUCH
            ||  coordinatesRecv[1]<=0
            ||  coordinatesRecv[1]>=XTOUCH
            ||  coordinatesRecv[2]<=0
            ||  coordinatesRecv[2]>=YTOUCH) { return 0; }


        actualCoordinates[0] =  -((XTOUCH*0.5)-coordinatesRecv[1]);
        actualCoordinates[1] =  -((YTOUCH*0.5)-coordinatesRecv[2]);

        return 1;
}
__attribute__ ((hot))
int ReadDSFlags(uint16_t* flagsRecv,uint16_t* actualFlags) {


    if (*flagsRecv==*actualFlags || *flagsRecv>=SHRT_MAX) {
        return 0;
    }
    *actualFlags= *flagsRecv;
    return 1;

}

__attribute__ ((hot))
int ReadDSInputInfo (int DSSocket,uint16_t* flagsBuffer,short coordinatesBuffer[]) {

    uint16_t buffer[3];
    int bytes_read;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(DSSocket, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        if (WSAGetLastError()!=WSAEWOULDBLOCK) {

            if (bytes_read == 0) {
                printf("[%d] Client socket closed connection.\n", DSSocket);
            }
            else {
                print_wsa_error("socket fd error");
            }
            return -1;
        }
        return 0;
    }
    ReadDSFlags(&buffer[0], flagsBuffer);
    if (!ReadDSScreenCoordinates(buffer, coordinatesBuffer)) {
        coordinatesBuffer[0]= NOTOUCH;
        coordinatesBuffer[1]= NOTOUCH;
    }
    return 1;
}
;




int ServerPart(uint32_t *PCIP) {
    printf("Entering server mode\n\n");
    int bytes_read;

    short posBuffer[2]= {0,0};
    uint16_t flags=0;
    int status;
    //struct sockaddr_in DSAdress;

    struct sockaddr_in PCPart;
    socklen_t ClientAdrSize;

    u_long nonblock = 1;

    struct sockaddr_in PCAdress;

    int SocketDS;

    //
    // fd_set all_sockets; // Ensemble de toutes les sockets du serveur
    // fd_set read_fds;    // Ensemble temporaire pour select()
    // int fd_max;         // Descripteur de la plus grande socket
    // struct timeval timer;




    // on prépare l'adresse et le port pour la socket de notre serveur

    struct sockaddr_in DSAdr;

    //memset(&DSAdr, 0, sizeof(DSAdr));
    //memset(&PCPart, 0, sizeof (PCPart));
    //memset(SocketDS, 0, sizeof (SocketDS));
    memset(&PCAdress, 0, sizeof (PCAdress));


    SocketDS=socket(AF_INET, SOCK_STREAM, 0);
    if (SocketDS == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        closesocket(SocketDS);
        return (1);
    }

    ioctlsocket(SocketDS, FIONBIO, &nonblock);


    int SocketPC = socket(AF_INET, SOCK_STREAM, 0);

    //ioctlsocket(SocketPC, FIONBIO, &nonblock);

    if (SocketPC == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        CloseAll(SocketDS, SocketPC);
        return (1);
    }


    printf("Created server socket fd: %d\n", SocketPC);
    char aa[24];

    PCAdress.sin_family = AF_INET;
    PCAdress.sin_addr.s_addr = *PCIP;
    PCAdress.sin_port = htons(4242);



    status = bind(SocketPC, (struct sockaddr *) &PCAdress, sizeof(PCAdress));
    if (status != 0) {
        print_wsa_error("socket fd error");
        CloseAll(SocketDS, SocketPC);
        return (2);
    }
    printf("Bound socket to localhost port %d\n", PCAdress.sin_port);


    printf("Listening on port %d\n", PCAdress.sin_port);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {

        print_wsa_error("socket fd error");
        CloseAll(SocketDS, SocketPC);
        return (3);
    }

    // FD_ZERO(&all_sockets);
    // FD_ZERO(&read_fds);
    // FD_SET(SocketPC, &all_sockets); // Ajout de la socket principale à l'ensemble
    // fd_max = SocketPC; // Le descripteur le plus grand est forcément celui de notre seule socket
    printf("[Server] Set up select fd sets\n");


    int lastCursorPos[2]= {0,0};
    int absoluteCursorPos[2];

    GetCursorPos(absoluteCursorPos);

    int DSConnected = 0;
    int waitingPos=0;

    INPUT inputs[0];
    ZeroMemory(inputs, sizeof(inputs));
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    INT32 DSAdrLength = sizeof DSAdr;

    uint16_t lastFlagsValue=flags;

    struct pollfd poll_fd[1];

    poll_fd[0].fd = SocketPC;
    poll_fd[0].events = POLLIN | POLLOUT;




    while (!kbhit())
        {
            if (!DSConnected) {
                status = WSAPoll(poll_fd, 1, 1000);
                if (status<0) {
                    print_wsa_error("Poll error");
                }if (status==0) {
                    printf("waiting\n");
                }
                if (((status) > 0)) {
                    SocketDS = accept(SocketPC, (struct sockaddr *) &DSAdr, &DSAdrLength);
                    if (SocketDS < 0) {

                        print_wsa_error("socket fd error");
                        CloseAll(SocketDS, SocketPC);
                        return (1);

                    }
                    DSConnected = 1;
                    printf("You can now use your 3DS as a controller\n");

                }
            }
            else {

                status=ReadDSInputInfo(SocketDS, &flags, posBuffer);
                printf("\rFlags :  %d", flags);
                if (status) {
                    // if (FD_ISSET(i,&all_sockets)) {
                    if ((flags & 0b1000) && posBuffer[0]!=NOTOUCH) {

                        // printf("posX %hd\n",posBuffer[1]);
                        // printf("posY %hd\n",posBuffer[0]);

                        absoluteCursorPos[0] = posBuffer[0]+lastCursorPos[0];
                        absoluteCursorPos[1] = posBuffer[1]+lastCursorPos[1];

                        SetCursorPos(absoluteCursorPos[0], absoluteCursorPos[1]);
                    } else if (lastFlagsValue & 0b1000) {
                        lastCursorPos[0] = absoluteCursorPos[0];
                        lastCursorPos[1] = absoluteCursorPos[1];
                    }

                    if (flags != lastFlagsValue) {
                        //printf("Flags %hu\n",flags);
                        if ((flags & 0b0001)) {
                            inputs[0].type = INPUT_MOUSE;
                            inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                            SendInput(1, &inputs, sizeof(INPUT));
                            // printf("bufferX %hd\n",posBuffer[0]);
                            // printf("bufferY %hd\n",posBuffer[1]);
                            // printf("posX %hd\n",absoluteCursorPos[0]);
                            // printf("posY %hd\n",absoluteCursorPos[1]);

                        }
                        else if ((!(flags & 0b0001)) && (lastFlagsValue & 0b0001)) {
                            inputs[0].type = INPUT_MOUSE;
                            inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
                            SendInput(1, &inputs, sizeof(INPUT));
                            // printf("bufferXA %hd\n",posBuffer[0]);
                            // printf("bufferYA %hd\n",posBuffer[1]);
                            // printf("posXA %hd\n",absoluteCursorPos[0]);
                            // printf("posYA %hd\n",absoluteCursorPos[1]);
                        }
                        lastFlagsValue = flags;
                    }
                }else if (status<0) {
                    break;
                }
            }

    }

    CloseAll(SocketDS, SocketPC);
    return 0;
}


