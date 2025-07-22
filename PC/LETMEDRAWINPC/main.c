#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "test.h"



 // le port de notre serveur
#define BACKLOG 10 // nombre max de demandes de connexion



int main(void) {

    int SocketDS;
    int SocketPC;
    struct sockaddr_in DSAdr;
    struct sockaddr_in PCAdr;
    char buffer[BUFSIZ];
    int bytes_read;


    if (!ClientStart(&SocketDS, &DSAdr, buffer, &bytes_read))
    {
        printf("Closing socket\n");
        closesocket(SocketDS);
        WSACleanup();
        return (0);
    }

printf("---- SERVER ----\n\n");


    int status;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;

    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&PCAdr, 0, sizeof PCAdr);
    PCAdr.sin_family = AF_INET; // IPv4
    PCAdr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    PCAdr.sin_port = DSAdr.sin_port;


    // on crée la socket, on a lit et on écoute dessus
    SocketPC = socket(PCAdr.sin_family, SOCK_STREAM, 0);
    if (SocketPC == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (1);
    }
    printf("Created server socket fd: %d\n", SocketPC);

    status = bind(SocketPC, (struct sockaddr *)&PCAdr, sizeof PCAdr);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (2);
    }
    printf("Bound socket to localhost port %d\n", 8000);

    printf("Listening on port %d\n", 8000);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (3);
    }

    // on accepte une connexion entrante
    addr_size = sizeof DSAdr;
    SocketDS = accept(SocketPC, (struct sockaddr *)&client_addr, &addr_size);
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
            printf("Message received from client socket %d: \"%s\"\n", SocketDS, buffer);

            bytes_sent = send(SocketDS, msg, msg_len, 0);
            if (bytes_sent == SOCKET_ERROR) {
                print_wsa_error("socket fd error");
            }
            else if (bytes_sent == msg_len) {
                printf("Sent full message to client socket %d: \"%s\"\n", SocketDS, msg);
            }
            else {
                printf("Sent partial message to client socket %d: %d bytes sent.\n", SocketDS, bytes_sent);
            }
        }
    }

    printf("Closing client socket\n");
    closesocket(SocketDS);
    printf("Closing server socket\n");
    closesocket(SocketPC);
    WSACleanup();
    return 0;

}




