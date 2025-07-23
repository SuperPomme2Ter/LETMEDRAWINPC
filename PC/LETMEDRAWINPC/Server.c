//
// Created by rapha on 23/07/2025.
//
#include <stdio.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Server.h"

#define BACKLOG 10
#define SERVPORT "8000"

int ServerPart(struct addrinfo * PCAdress) {
    printf("---- SERVER ----\n\n");
    int bytes_read;
    char buffer[BUFSIZ];
    int status;
    struct sockaddr_storage client_addr;
    struct sockaddr_in PCPart;
    socklen_t ClientAdrSize;



    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&PCPart, 0, sizeof PCPart);
    memset(&client_addr, 0, sizeof (client_addr));


    // on crée la socket, on a lit et on écoute dessus
    int SocketPC = socket(PCAdress->ai_family, PCAdress->ai_socktype, PCAdress->ai_protocol);
    if (SocketPC == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (1);
    }
    printf("Created server socket fd: %d\n", SocketPC);
    char aa[24];

    inet_ntop(AF_INET,&(PCAdress->ai_addr),aa,24);
    printf("test : %s\n", aa);

    status = bind(SocketPC, PCAdress->ai_addr, PCAdress->ai_addrlen);
    if (status != 0) {
        freeaddrinfo(PCAdress);
        print_wsa_error("socket fd error");
        return (2);
    }
    printf("Bound socket to localhost port %d\n", 8000);

    freeaddrinfo(PCAdress);

    printf("Listening on port %d\n", 8000);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (3);
    }

    // on accepte une connexion entrante
    ClientAdrSize = sizeof (client_addr);
    int SocketDS = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketDS == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (1);
    }

    SocketDS = accept(SocketPC, (struct sockaddr *) &client_addr, &ClientAdrSize);
    if (SocketDS == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (4);
    }
    printf("Accepted new connection on client socket fd: %d\n", SocketDS);

    // on recoit un message via la socket client
    bytes_read = 1;
    while (bytes_read >= 0) {
        printf("Reading client socket %d\n", SocketDS);
        bytes_read = recv(SocketDS, buffer, BUFSIZ, 0);
        if (bytes_read == 0) {
            printf("Client socket %d: closed connection.\n", SocketDS);
            break ;
        } else if (bytes_read == SOCKET_ERROR) {
            print_wsa_error("socket fd error");
            break ;
        } else {
            // Si on a bien reçu un message, on va l'imprimer
            // puis renvoyer un message au client
            char *msg = "Got your message.";
            int msg_len = strlen(msg);
            int bytes_sent;

            buffer[bytes_read] = '\0';
            printf("Message received from client socket %d: \"%s\"\n", SocketDS, buffer);

            bytes_sent = send(SocketDS, msg, msg_len, 0);
            if (bytes_sent == SOCKET_ERROR) {
                print_wsa_error("socket fd error");
            } else if (bytes_sent == msg_len) {
                printf("Sent full message to client socket %d: \"%s\"\n", SocketDS, msg);
            } else {
                printf("Sent partial message to client socket %d: %d bytes sent.\n", SocketDS, bytes_sent);
            }
        }
    }


    printf("Closing client socket\n");
    closesocket(SocketDS);
    printf("Closing server socket\n");
    closesocket(SocketPC);
    return 0;
}
