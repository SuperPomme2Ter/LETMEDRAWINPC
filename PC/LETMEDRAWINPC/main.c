#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Client.h"
#include "Server.h"





// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH

int main(void) {


    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 0;
    }

    char host[256];
    char *IP;
    struct hostent *host_entry;

    int hostname;

    // it will find the host name of current machine
    hostname = gethostname(host, sizeof(host));

    host_entry = gethostbyname(host);
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    struct sockaddr_in DSAdr;

    struct addrinfo *PCAdrInfo;


    if (!ClientStart(&DSAdr))
    {
        printf("Closing socket\n");
        WSACleanup();
        return (0);
    }
    ServerPart(IP);
    WSACleanup();



    return 0;

}






