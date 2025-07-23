#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Client.h"
#include "Server.h"



// le port de notre serveur
 // nombre max de demandes de connexion



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

    // it will find the host information
    host_entry = gethostbyname(host);
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    // It will convert into IP string
    printf("Current Host Name: %s\n", host);
    printf("Host IP: %s\n", IP);



    struct sockaddr_in DSAdr;


    struct addrinfo *PCAdrInfo;
    if (!ClientStart(&DSAdr,IP))
    {
        printf("Closing socket\n");
        WSACleanup();
        return (0);
    }
 //   ServerPart();
    WSACleanup();
    return 0;

}




