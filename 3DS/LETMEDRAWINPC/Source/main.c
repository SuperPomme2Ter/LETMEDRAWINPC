#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>


#include <fcntl.h>


#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <3ds.h>
#include "FlagStructData.h"




#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

#define NOTOUCH 999

// flags for inputs from 3DS to send to PC
// explicit conversion to reduce the size of the data to send, int is 4bytes, short is 2


short GetBitshiftingOccurence(u_int16_t bitmask);

u_int16_t From3DSFlagsToAppFlags(u_int16_t flags3ds);


static u32 *SOC_buffer = NULL;
s32 DSServerSocket = -1, PCClientSocket = -1, PCServerSocket=-1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

void PrintWarning(const char *fmt, ...);

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

    uint16_t keyFlags = 0b000000000000;


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
    memset(&PCAdr, 0, sizeof (PCAdr));

    server.sin_family = AF_INET;
    server.sin_port = htons(4242);
    server.sin_addr.s_addr = gethostid();



    DSServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (DSServerSocket < 0) {
        failExit("socket: %d %s\n", errno, strerror(errno));
    }


    if ((bind(DSServerSocket, (struct sockaddr *) &server, sizeof (server)))<0) {
        close(DSServerSocket);
        failExit("bind: %d %s\n", errno, strerror(errno));
    }

    if ((listen(DSServerSocket, 5))<0) {
        failExit("listen: %d %s\n", errno, strerror(errno));
    }

    printf("3DS IP Adress %s\n", inet_ntoa(server.sin_addr));
    printf("3DS Listening\n");


    //printf("aaaa %d\n", ConvertToAppFlag(0b11111111111111111111111111111111));

    u32 kDown;
    uint16_t lastFlag = keyFlags;
    u32 kHeld;
    u32 kUp;
    touchPosition touch;

    u16 inputInfo[3] = {
        keyFlags,
        0,
        0
    };

    fcntl(PCClientSocket, F_SETFL, fcntl(PCClientSocket, F_GETFL, 0) | O_NONBLOCK);
    fcntl(DSServerSocket, F_SETFL, fcntl(DSServerSocket, F_GETFL, 0) | O_NONBLOCK);

    // client.sin_family = AF_INET;
    // client.sin_port = htons(4242);

    struct pollfd poll_fds[1];



    while (aptMainLoop()) {
        hidScanInput();
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        kUp = hidKeysUp();
        gspWaitForVBlank();
        if (kDown & KEY_START) break;

        if (socketMode == -1) {
            PCClientSocket = accept(DSServerSocket, (struct sockaddr *) &client, &clientlen);
            if (PCClientSocket < 0) {
                if (errno != EAGAIN) {
                    failExit("accept: %d %s\n", errno, strerror(errno));
                } else {

                    continue;
                }

            }else {
                printf("PORT : %hu\n", server.sin_port);
                printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));

                PCAdr = client;
                PCAdr.sin_family = AF_INET;
                PCAdr.sin_port = htons(4242);

                shutdown(PCClientSocket,SHUT_RDWR);
                close(PCClientSocket);
                shutdown(DSServerSocket,SHUT_RDWR);
                close(DSServerSocket);
            }
            printf("Entering Client Mode\n\n");
            socketMode = 0;
            PCAdr.sin_family = AF_INET;
            PCAdr.sin_port = htons(4242);
            PCServerSocket = socket(AF_INET, SOCK_STREAM, 0);
            fcntl(PCServerSocket, F_SETFL, O_NONBLOCK);
        }
        else if (socketMode == 0) {

                status = connect(PCServerSocket, (struct sockaddr *) &PCAdr, sizeof PCAdr);
                if ((status != 0) && (errno != EINPROGRESS)) {
                    printf(" port : %d\n", PCAdr.sin_port);
                    printf(" IP : %s\n", inet_ntoa(PCAdr.sin_addr));
                    printf("Failed to connect \n");
                    close(PCServerSocket);
                    failExit("connect: %d %s\n", errno, strerror(errno));
                } else {
                    if (errno == EINPROGRESS) {
                        PrintWarning("connection in progress\n\n");
                        printf(" port : %d\n", PCAdr.sin_port);
                        printf(" IP : %s\n", inet_ntoa(PCAdr.sin_addr));
                    }//else {
                        printf("Connected\n\n");
                        printf("Touchscreen work as your mouse\n");
                        socketMode = 1;
                        inputInfo[1] = NOTOUCH;
                        inputInfo[2] = NOTOUCH;
                        printf("%i",1 & ~TOUCHSCREEN);
                    //}

                }


        } else if (socketMode == 1) {

            keyFlags |= ConvertToAppFlag(kDown);
            keyFlags &= ~ConvertToAppFlag(kUp);




            if (!(keyFlags & TOUCHSCREEN)) {
                inputInfo[1] = NOTOUCH;
                inputInfo[2] = NOTOUCH;
                if ((keyFlags != lastFlag)) {
                    inputInfo[0] = keyFlags;
                    lastFlag = keyFlags;
                    status = send(PCServerSocket, inputInfo, sizeof (inputInfo), 0);
                    if (status != sizeof(inputInfo)) {
                        failExit("send: %d %s\n", errno, strerror(errno));
                    }
                }
            }
            else {
                inputInfo[0] = keyFlags;

                // if ((lastFlag & TOUCHSCREEN) && (keyFlags==0)) {
                //     inputInfo[1] = NOTOUCH;
                //     inputInfo[2] = NOTOUCH;
                //     lastFlag = keyFlags;
                //     status = send(PCServerSocket, inputInfo, sizeof (inputInfo), 0);
                //     if (status != sizeof(inputInfo)) {
                //
                //         failExit("send: %d %s\n", errno, strerror(errno));
                //     }
                //     printf("fais chier\n");
                //     continue;
                // }
                lastFlag = keyFlags;

                //Read the touch screen coordinates
                hidTouchRead(&touch);

                inputInfo[1] = touch.px;
                inputInfo[2] = touch.py;
                if (keyFlags & TOUCHSCREEN) {
                    status = send(PCServerSocket, inputInfo, sizeof (inputInfo), 0);
                    if (status != sizeof(inputInfo)) {
                        printf("JE T'EMMERDE\n");
                        failExit("send: %d %s\n", errno, strerror(errno));
                    }
                }

            }
            // if ((keyFlags != lastFlag)) {
            //     lastFlag = keyFlags;
            //     printf("flags : %i \n",keyFlags);
            //     printf("lastFlag : %i \n",lastFlag);
            // }

            //
            // if (kHeld & KEY_TOUCH) {
            //
            // }
        }

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    close(DSServerSocket);
    close(PCClientSocket);
    close(PCServerSocket);
    return 0;
}

short GetBitshiftingOccurence(u_int16_t bitmask) {
    short occurence=1;
    u_int16_t testingMask=1;
    for (short i = 0; i < 16; i++) {
        testingMask <<= 1;
        occurence ++;
        if (bitmask & testingMask) {
            return occurence;
        }
    }
    if (occurence >= 16) {
        return -1;
    }

}

//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...) {
    //---------------------------------------------------------------------------------

    close(DSServerSocket);
    close(PCClientSocket);
    close(PCServerSocket);

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


void PrintWarning(const char *fmt, ...) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "Warning : %s", fmt);

    va_list ap;

    printf(CONSOLE_YELLOW);
    va_start(ap, buffer);
    vprintf(buffer, ap);
    va_end(ap);
    printf(CONSOLE_RESET);
    printf("\n");
}
