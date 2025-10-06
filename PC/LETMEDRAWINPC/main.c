#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Debug.h"
#include "Client.h"
#include "Server.h"
#include <windows.h>
#include "cJSON.h"

#define ABUTTON 0
#define BBUTTON 1
#define XBUTTON 2
#define YBUTTON 3
#define LBUTTON 4
#define RBUTTON 5
#define UPBUTTON 6
#define DOWNBUTTON 7
#define LEFTBUTTON 8
#define RIGHTBUTTON 9
#define SELECTBUTTON 10





// POUR LE SAINT AMOUR DES VECTREX REFACTO ET NETTOIE MOI CE CODE FOU QUE JE SUIS AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH

int main(void) {
    int readingSucessful=1;
        // if (readingSucessful) {
        //     FILE *fp = fopen("Configuration.json", "r");
        //     if (fp == NULL) {
        //         printf("Error: Unable to open the file.\n");
        //         readingSucessful=0;
        //         continue;
        //     }
        //     char buffer[1024];
        //     int len = fread(buffer,1, sizeof(buffer), fp);
        //     fclose(fp);
        //
        //     // parse the JSON data
        //     cJSON *json = cJSON_Parse(buffer);
        //     if (json == NULL) {
        //         const char *error_ptr = cJSON_GetErrorPtr();
        //         if (error_ptr != NULL) {
        //             printf("Error: %s\n", error_ptr);
        //         }
        //         cJSON_Delete(json);
        //         readingSucessful=0;
        //         continue;
        //     }
        //
        //     // access the JSON data
        //     cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
        //     if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        //         printf("Name: %s\n", name->valuestring);
        //     }
        //
        //
        //     // delete the JSON object
        //     cJSON_Delete(json);
        //     break;
        // }


     WSADATA wsaData;
     if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
         printf("WSAStartup failed\n");
         return 0;
     }

     char host[256];

     struct hostent *host_entry;

     int hostname;

     // it will find the host name of current machine
     hostname = gethostname(host, sizeof host);

     host_entry = gethostbyname(host);
     uint32_t  *IP= (uint32_t *) host_entry->h_addr_list[0];


     struct sockaddr_in DSAdr;

     memset(&DSAdr, 0, sizeof(DSAdr));



     struct addrinfo *PCAdrInfo;


     if (!ClientStart())
     {
         printf("Closing socket\n");
         WSACleanup();
         return (0);
     }
     ServerPart(IP);
     WSACleanup();



     return 0;

 }

