// client.c - un micro-client qui envoie un message à un serveur et attend DSAdr réponse

#include <conio.h>
#include <stdio.h>
#include <ws2tcpip.h>

#include "Debug.h"


#define SERVPORT 8000

int ClientStart() {
    printf("REMEMBER TO LAUNCH THE APP IN YOUR CONSOLE BEFORE ENTERING THE IP ADRESS !\n\n");
    //printf("ENTER 3DS IP ADRESS\n\n");
    int status;


    int a = 1;
    char ServerAdrStr[24];
    int bytes_read;
    char buffer[BUFSIZ];


    struct sockaddr_in DSAdr;

    memset(&DSAdr, 0, sizeof(DSAdr));

    scanf("%s", ServerAdrStr);
    a = inet_pton(AF_INET, ServerAdrStr, &(DSAdr.sin_addr)); //DS address

    DSAdr.sin_family = AF_INET; // IPv4
    DSAdr.sin_port = htons(4242);

    if (a == -1) {
        printf("Hate you\n");
    }
    if (a == 0) {
        printf("adresse marche pas :(\n");
        printf("Adresse : %s", ServerAdrStr);
    }

    char aa[24];
    inet_ntop(AF_INET, &(DSAdr.sin_addr), aa, 24);

    // on crée la socket et on la connecte au serveur distant

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == SOCKET_ERROR) {
        Print_wsa_error("socket fd error");
        shutdown(socket_fd,SD_BOTH);
        closesocket(socket_fd);
        return (0);
    }

    printf("Created socket fd: %d\n", socket_fd);

    status = connect(socket_fd, (const struct sockaddr *) &DSAdr, sizeof DSAdr);
    if (status != 0) {
        Print_wsa_error("socket fd error");
        shutdown(socket_fd,SD_BOTH);
        closesocket(socket_fd);
        return (0);
    }
    printf("Connected socket to localhost port %d\n", DSAdr.sin_port);
    shutdown(socket_fd,SD_BOTH);
    closesocket(socket_fd);
    return (1);


}
