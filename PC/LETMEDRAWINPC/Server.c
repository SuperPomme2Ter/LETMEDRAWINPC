//
// Created by rapha on 23/07/2025.
//
#include <stdio.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Server.h"

#include <stdint.h>

#define BACKLOG 10
#define SERVPORT "8000"



void read_data_from_socket(int socket, fd_set *all_sockets, int fd_max, int server_socket,char* textBuffer)
{
    uint16_t buffer[2];
    int bytes_read;
    int status;
    uint16_t lastbuffer[2];

    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(socket, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            printf("[%d] Client socket closed connection.\n", socket);
        }
        else {
            print_wsa_error("socket fd error");
        }
        FD_CLR(socket, all_sockets); // Enlève la socket de l'ensemble
        close(socket); // Ferme la socket
    }
    else {
        // Renvoie le message reçu à toutes les sockets connectées
        // à part celle du serveur et celle qui l'a envoyée
        if (buffer[0]!=lastbuffer[0]) {
            printf("    px: %hu ",buffer[0]);
            printf(" py: %hu\n",buffer[1]);
            lastbuffer[0] = buffer[0];
        }


    }

}

void accept_new_connection(int server_socket, fd_set *all_sockets, int *fd_max)
{
    int client_fd;
    int status;

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

int ServerPart(char* PCIP) {
    printf("---- SERVER ----\n\n");
    int bytes_read;

    char buffer[BUFSIZ];
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
        return (2);
    }
    printf("Bound socket to localhost port %d\n", 8000);


    printf("Listening on port %d\n", 8000);
    status = listen(SocketPC, BACKLOG);
    if (status != 0) {
        print_wsa_error("socket fd error");
        return (3);
    }
    FD_ZERO(&all_sockets);
    FD_ZERO(&read_fds);
    FD_SET(SocketPC, &all_sockets); // Ajout de la socket principale à l'ensemble
    fd_max = SocketPC; // Le descripteur le plus grand est forcément celui de notre seule socket
    printf("[Server] Set up select fd sets\n");

    // on accepte une connexion entrante
    /*ClientAdrSize = sizeof (client_addr);
    int SocketDS = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketDS == SOCKET_ERROR) {
        print_wsa_error("socket fd error");
        return (1);
    }*/

    int DSConnected = 0;
    while (1)
        {
        read_fds = all_sockets;
        // Timeout de 2 secondes pour select()
        timer.tv_sec = 2;
        timer.tv_usec = 0;

        // Surveille les sockets prêtes à être lues
        status = select(fd_max + 1, &read_fds, NULL, NULL, &timer);
        if (status == -1) {
            print_wsa_error("socket fd error");
            exit(1);
        }
        else if (status == 0) {
            // Aucun descipteur de fichier de socket n'est prêt pour la lecture
            printf("[Server] Waiting...\n");
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
            if (DSConnected==0) {
                if (i == SocketPC) {
                    // La socket est notre socket serveur qui écoute le port
                    accept_new_connection(SocketPC, &all_sockets, &fd_max);
                    DSConnected = 1;
                    FD_CLR(SocketPC, &all_sockets); // Enlève la socket de l'ensemble
                }
            }

            else {
                // La socket est une socket client, on va la lire
                read_data_from_socket(i, &all_sockets, fd_max, SocketPC,buffer);
                //system("cls");
                //printf("buffer : %s\n", buffer);

            }
        }
    }


    printf("Closing client socket\n");
    for (int i = 0; i <= fd_max; i++) {
        close(i); // Ferme la socket
        FD_CLR(i, &all_sockets); // Enlève la socket de l'ensemble
    }
    printf("Closing server socket\n");
    closesocket(SocketPC);
    return 0;
}
