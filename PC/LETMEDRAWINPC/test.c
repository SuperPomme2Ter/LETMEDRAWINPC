// client.c - un micro-client qui envoie un message à un serveur et attend DSAdr réponse

#include <stdio.h>
#include <ws2tcpip.h>

#include "Debug.h"

#define SERVPORT 8000


int ClientStart(int* socket_fd)
{
        WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    printf("---- CLIENT ----\n\n");
    struct sockaddr_in DSAdr;
    int status;
    char buffer[BUFSIZ];
    int bytes_read;
    int a=1;
    char ServerAdrStr[24];

    // on prépare l'adresse et le port auquel on veut se connecter

    memset(&DSAdr, 0, sizeof DSAdr);
    scanf( "%s", ServerAdrStr);
    a=inet_pton(AF_INET, ServerAdrStr, &(DSAdr.sin_addr)); //DS adress
    DSAdr.sin_family = AF_INET; // IPv4
    DSAdr.sin_port = htons(SERVPORT);
    if (a==-1) {
        printf("Hate you\n");
    }
    if (a==0) {

        printf("adresse marche pas :(\n");
        printf("Adresse : %s",ServerAdrStr);
    }
    char aa[24];
    inet_ntop(AF_INET,&(DSAdr.sin_addr),aa,24);






    // on crée la socket et on la connecte au serveur distant
    *socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*socket_fd == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (2);
    }
    printf("Created socket fd: %d\n", *socket_fd);

    status = connect(*socket_fd, (const struct sockaddr *)&DSAdr, sizeof DSAdr);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (3);
    }
    printf("Connected socket to localhost port %d\n", SERVPORT);


    // on attend de recevoir un message via la socket
    int succeded=0;
    bytes_read = 1;
    while (bytes_read >= 0) {
        bytes_read = recv(*socket_fd, buffer, BUFSIZ, 0);
        if (bytes_read == 0) {
            printf("Server closed connection.\n");
            break ;
        }
        else if (bytes_read == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            break ;
        }
        else {
            // Si on a bien reçu un message, on va l'imprimer
            buffer[bytes_read] = '\0';
            printf("Message received: \"%s\"\n", buffer);
            succeded = 1;
            break ;
        }
    }

    return (succeded);

}



