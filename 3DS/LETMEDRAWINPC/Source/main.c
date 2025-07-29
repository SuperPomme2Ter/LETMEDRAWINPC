

#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <3ds.h>

#include "Controls.h"

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 DSServerSocket = -1, PCClientSocket = -1, PCServerSocket;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

//---------------------------------------------------------------------------------
void socShutdown() {
    //---------------------------------------------------------------------------------
    printf("waiting for socExit...\n");
    socExit();
}



//---------------------------------------------------------------------------------
int main(int argc, char **argv) {

    //---------------------------------------------------------------------------------
    int ret;
    int socketMode = -1;
    u32 clientlen;
    struct sockaddr_in client;
    struct sockaddr_in server;
    struct sockaddr_in PCAdr;
    int status;


    gfxInitDefault();
    // register gfxExit to be run when app quits
    // this can help simplify error handling
    atexit(gfxExit);

    consoleInit(GFX_TOP, NULL);

    // allocate buffer for SOC service
    SOC_buffer = (u32 *) memalign(SOC_ALIGN, SOC_BUFFERSIZE);

    if (SOC_buffer == NULL) {
        failExit("memalign: failed to allocate\n");
    }

    // Now intialise soc:u service
    if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
        failExit("socInit: 0x%08X\n", (unsigned int) ret);
    }

    // register socShutdown to run at exit
    // atexit functions execute in reverse order so this runs before gfxExit
    atexit(socShutdown);

    // libctru provides BSD sockets so most code from here is standard
    clientlen = sizeof(client);


    memset(&server, 0, sizeof (server));
    memset(&client, 0, sizeof (client));

    server.sin_family = AF_INET;
    server.sin_port = htons(8000);
    server.sin_addr.s_addr = gethostid();


    DSServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (DSServerSocket < 0) {
        failExit("socket: %d %s\n", errno, strerror(errno));
    }

    if ((ret = bind(DSServerSocket, (struct sockaddr *) &server, sizeof (server)))) {
        close(DSServerSocket);
        failExit("bind: %d %s\n", errno, strerror(errno));
    }

    if ((ret = listen(DSServerSocket, 5))) {
        failExit("listen: %d %s\n", errno, strerror(errno));
    }

    printf("3DS IP Adress %s\n", inet_ntoa(server.sin_addr));
    printf("3DS Listening\n");

    u32 kDown;

    while (aptMainLoop()) {

        hidScanInput();
        kDown = hidKeysDown();
        if (kDown & KEY_START) break;

        if (socketMode==-1) {
            PCClientSocket = accept(DSServerSocket, (struct sockaddr *) &client, &clientlen);
            memset(&PCAdr, 0, sizeof (PCAdr));
            PCAdr=client;
            if (PCClientSocket < 0) {
                if (errno != EAGAIN) {
                    failExit("accept: %d %s\n", errno, strerror(errno));
                }
            } else {
                printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
            }
            printf("Entering Client Mode\n");
            socketMode = 0;
            PCAdr.sin_family = AF_INET;
            PCAdr.sin_port = htons(8000);

            PCServerSocket = socket(AF_INET, SOCK_STREAM, 0);
            fcntl(PCServerSocket, F_SETFL, O_NONBLOCK);

        }

        else if (socketMode == 0) {




            status = connect(PCServerSocket, (struct sockaddr *) &PCAdr, sizeof (PCAdr));
            if (status != 0) {
                printf("Failed to connect \n");
                failExit("connect: %d %s\n", errno, strerror(errno));
            }
            else {
                printf("Connected\n");
                socketMode = 1;

            }

        } else if (socketMode == 1) {

            //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)


            touchPosition touch;

            //Read the touch screen coordinates
            hidTouchRead(&touch);

            //Print the touch screen coordinates


            u16 positionToSend[]={
                touch.px,
                touch.py
            };
            printf("\x1b[8;0H%03d; %03d", positionToSend[0], positionToSend[1]);

            status=send(PCServerSocket, positionToSend, sizeof (positionToSend),0);
            if (status!=sizeof(positionToSend)) {
                failExit("send: %d %s\n", errno, strerror(errno));
            }


            // Flush and swap framebuffers
            gfxFlushBuffers();
            gfxSwapBuffers();
        }

    }
    if (socketMode == -1) {
        close(PCClientSocket);
        close(DSServerSocket);
    } else {
        close(PCServerSocket);
    }
    /*int Amount=500;
    send(newSocket, &Amount, sizeof(Amount), 0);
    close(welcomeSocket);

    recv(clientSocket, &Amount, sizeof(Amount), 0);
    // Amount= ntohl(Amount);
    printf("Data received: %d\n",Amount));*/

    return 0;
}


//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...) {
    //---------------------------------------------------------------------------------

    if (DSServerSocket > 0) close(DSServerSocket);
    if (PCClientSocket > 0) close(PCClientSocket);
    if (PCServerSocket > 0) close(PCServerSocket);

    va_list ap;

    printf(CONSOLE_RED);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf(CONSOLE_RESET);
    printf("\nPress B to exit\n");

    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_B) exit(0);
    }
}
