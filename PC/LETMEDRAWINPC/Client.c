// client.c - un micro-client qui envoie un message à un serveur et attend DSAdr réponse

#include <stdio.h>
#include <ws2tcpip.h>

#include "Debug.h"


#define SERVPORT 8000

int ClientStart(struct sockaddr_in *DSAdr) {
    printf("---- CLIENT ----\n\n");
    int status;


    int a = 1;
    char ServerAdrStr[24];
    int bytes_read;
    char buffer[BUFSIZ];


    memset(DSAdr, 0, sizeof *DSAdr);

    scanf("%s", ServerAdrStr);
    a = inet_pton(AF_INET, ServerAdrStr, &(DSAdr->sin_addr)); //DS adresse

    DSAdr->sin_family = AF_INET; // IPv4
    DSAdr->sin_port = htons(SERVPORT);

    if (a == -1) {
        printf("Hate you\n");
    }
    if (a == 0) {
        printf("adresse marche pas :(\n");
        printf("Adresse : %s", ServerAdrStr);
    }

    char aa[24];
    inet_ntop(AF_INET, &(DSAdr->sin_addr), aa, 24);

    // on crée la socket et on la connecte au serveur distant

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (0);
    }

    printf("Created socket fd: %d\n", socket_fd);

    status = connect(socket_fd, (const struct sockaddr *) DSAdr, sizeof *DSAdr);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (0);
    }
    printf("Connected socket to localhost port %d\n", SERVPORT);


    // on attend de recevoir un message via la socket





        /*status=send(socket_fd, IP, strlen(IP) , 0);
        if (status == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            return 0 ;
        }
        else {
            printf("Client socket %d: reçu IP\n", socket_fd);
            msgSuccess++;
        }*/



        closesocket(socket_fd);
    return (1);


}
