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

int ReadDSFlags(uint16_t* flagsRecv,uint8_t* actualFlags) {


    if (*flagsRecv==*actualFlags || *flagsRecv>=SHRT_MAX) {
        return 0;
    }
    *actualFlags= *flagsRecv;
    return 1;

}

int ReadDSInputInfo(int DSSocket, fd_set *all_sockets,uint8_t* flagsBuffer,short coordinatesBuffer[]) {

    uint16_t buffer[3];
    int bytes_read;

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(DSSocket, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", DSSocket);
        }
        else {
            print_wsa_error("socket fd error");
        }
        FD_CLR(DSSocket, all_sockets); // Enlève la socket de l'ensemble
        close(DSSocket); // Ferme la socket
        return 0;
    }
    ReadDSFlags(&buffer[0], flagsBuffer);
    if (!ReadDSScreenCoordinates(buffer, coordinatesBuffer)) {
        coordinatesBuffer[0]= NOTOUCH;
        coordinatesBuffer[1]= NOTOUCH;
    }
    return 1;



}

void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)
{
    int client_fd;
    int status;
    //printf("J'en peut plus\n");

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
        print_wsa_error("socket fd error");
        return ;
    }
    FD_SET(client_fd, all_sockets); // Ajoute la socket client à l'ensemble
    if (client_fd > *fd_max) {
        *fd_max = client_fd; // Met à jour la plus grande socket
    }
    printf("[Server] Accepted new connection on client socket %d.\n", client_fd);

}


void CloseAll(int fd_max,fd_set *all_sockets, int SocketPC) {

    printf("Closing client socket\n");
    for (int i = 0; i <= fd_max; i++) {
        close(i); // Ferme la socket
        FD_CLR(i, all_sockets); // Enlève la socket de l'ensemble
    }

    printf("Closing server socket\n");
    closesocket(SocketPC);
}

int ServerPart(char* PCIP) {
    printf("Entering server mode\n\n");
    int bytes_read;

    short posBuffer[2]= {0,0};
    uint8_t flags=0b00000000;
    int status;
    struct sockaddr_storage client_addr;
    struct sockaddr_in PCPart;
    socklen_t ClientAdrSize;

    struct sockaddr_in PCAdress;

    fd_set all_sockets; // Ensemble de toutes les sockets du serveur
    fd_set read_fds;    // Ensemble temporaire pour select()
    int fd_max;         // Descripteur de la plus grande socket
    struct timeval timer;


    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&PCPart, 0, sizeof PCPart);
    memset(&client_addr, 0, sizeof (client_addr));
    memset(&PCAdress, 0, sizeof (PCAdress));


    // on crée la socket, on a lit et on écoute dessus
    int SocketPC = socket(AF_INET, SOCK_STREAM, 0);

    if (SocketPC == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (1);
    }
    printf("Created server socket fd: %d\n", SocketPC);
    char aa[24];

    PCAdress.sin_family = AF_INET;
    PCAdress.sin_port = htons(8000);
    inet_pton(AF_INET,PCIP,&(PCAdress.sin_addr));

    status = bind(SocketPC, (struct sockaddr *) &PCAdress, sizeof(PCAdress));
    if (status != 0) {
        print_wsa_error("socket fd error");
        closesocket(SocketPC);
        return (2);
    }
    printf("Bound socket to localhost port %d\n", 8000);


    printf("Listening on port %d\n", 8000);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {
        closesocket(SocketPC);
        print_wsa_error("socket fd error");
        return (3);
    }
    FD_ZERO(&all_sockets);
    FD_ZERO(&read_fds);
    FD_SET(SocketPC, &all_sockets); // Ajout de la socket principale à l'ensemble
    fd_max = SocketPC; // Le descripteur le plus grand est forcément celui de notre seule socket
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

    uint8_t lastFlagsValue=flags;

    while (!kbhit())
        {
        read_fds = all_sockets;
        // Timeout de 2 secondes pour select()
        timer.tv_sec = 2;
        timer.tv_usec = 0;

        // Surveille les sockets prêtes à être lues
        status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
        if (status == -1) {
            closesocket(SocketPC);
            print_wsa_error("socket fd error");
            CloseAll(fd_max, &all_sockets, SocketPC);
            exit(1);
        }
        else if (status == 0) {
            // Aucun descipteur de fichier de socket n'est prêt pour la lecture
            //printf("[Server] Waiting...\n");
            continue;
        }

        // Boucle sur nos sockets
        for (int i = 0; i <= fd_max; i++) {
            if (FD_ISSET(i, &read_fds) != 1) {
                // Le fd i n'est pas une socket à surveiller
                // on s'arrête là et on continue la boucle
                continue ;
            }
            //printf("[%d] Ready for I/O operation\n", i);
            // La socket est prête à être lue !
            if (!DSConnected) {
                if (i == SocketPC) {
                    // La socket est notre socket serveur qui écoute le port

                    accept_new_connection(SocketPC, &all_sockets, &fd_max);
                    DSConnected = 1;
                    printf("You can now use your 3DS as a controller\n");
                    FD_CLR(SocketPC, &all_sockets); // Enlève la socket de l'ensemble
                }
            }

            else {


                ReadDSInputInfo(i, &all_sockets, &flags, posBuffer);
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
            }
        }
    }


    CloseAll(fd_max, &all_sockets, SocketPC);
    return 0;
}


