// client.c - un micro-client qui envoie un message à un serveur et attend DSAdr réponse

#include <stdio.h>
#include <ws2tcpip.h>

#include "Debug.h"


#define SERVPORT 8000

int ClientStart(struct sockaddr_in *DSAdr, char *IP) {
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
    int succeded = 0;
    bytes_read = 1;
    bytes_read = 1;
    while (bytes_read >= 0) {
        printf("Reading client socket %d\n", socket_fd);
        bytes_read = recv(socket_fd, buffer, BUFSIZ, 0);
        if (bytes_read == 0) {
            printf("Client socket %d: closed connection.\n", socket_fd);
            break ;
        }
        else if (bytes_read == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            break ;
        }
        else {
            // Si on a bien reçu un message, on va l'imprimer
            // puis renvoyer un message au client
            char *msg = "Got your message.";
            int msg_len = strlen(msg);
            int bytes_sent;

            buffer[bytes_read] = '\0';
            printf("Message received from client socket %d: \"%s\"\n", socket_fd, buffer);

            bytes_sent = send(socket_fd, msg, msg_len, 0);
            if (bytes_sent == SOCKET_ERROR) {
                print_wsa_error("socket fd error");
            }
            else if (bytes_sent == msg_len) {
                printf("Sent full message to client socket %d: \"%s\"\n", socket_fd, msg);
            }
            else {
                printf("Sent partial message to client socket %d: %d bytes sent.\n", socket_fd, bytes_sent);
            }
            break;
        }
    }
    while (1) {
        int IPLength = strlen(IP);
        status = send(socket_fd, &IPLength, sizeof(int), 0);
        if (status == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            return (0);
        }
        if (status == 0) {
            printf("Client socket %d: closed connection.\n", socket_fd);
            return 0 ;
        } else {
            printf("Client socket %d: recu IPLength\n", socket_fd);
            break;
        }
    }

        /*status=send(socket_fd, IP, strlen(IP) , 0);
        if (status == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            return 0 ;
        }
        else {
            printf("Client socket %d: reçu IP\n", socket_fd);
            msgSuccess++;
        }*/

        succeded = 1;


        closesocket(socket_fd);
        return (succeded);

}
