

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


#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

#define NOTOUCH 999



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

    uint8_t keyFlags=0b00000000;


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
    uint8_t lastFlag=keyFlags;
    u32 kHeld;
    u32 kUp;
    touchPosition touch;

    u16 inputInfo[3]= {
        keyFlags,
        0,
        0
    };

    fcntl(PCClientSocket, F_SETFL, fcntl(PCClientSocket, F_GETFL, 0) | O_NONBLOCK);
    fcntl(DSServerSocket, F_SETFL, fcntl(DSServerSocket, F_GETFL, 0) | O_NONBLOCK);

    while (aptMainLoop()) {

        hidScanInput();
        kDown = hidKeysDown();
        kHeld=hidKeysHeld();
        kUp=hidKeysUp();
        gspWaitForVBlank();
        if (kDown & KEY_START) break;

        if (socketMode==-1) {
            PCClientSocket = accept(DSServerSocket, (struct sockaddr *) &client, &clientlen);
            if (PCClientSocket < 0) {
                if (errno != EAGAIN) {
                    failExit("accept: %d %s\n", errno, strerror(errno));
                }else {
                    continue;
                }
            } else {
                printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
                memset(&PCAdr, 0, sizeof (PCAdr));
                PCAdr=client;
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
                printf("Connected\n\n");
                printf("Remember : the touchscreen work as your mouse and the A button simulate a left click\n");
                socketMode = 1;
                inputInfo[1]=NOTOUCH;
                inputInfo[2]=NOTOUCH;

            }

        } else if (socketMode == 1) {

            if (kDown & KEY_A)
            {
                keyFlags |= 0b0001;
            }
            if (kUp & KEY_A) {
                keyFlags &= ~0b0001;
            }



            //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
            if (kDown & KEY_TOUCH){
                keyFlags |= 0b1000;
            }
            if (kUp & KEY_TOUCH) {
                inputInfo[1]=NOTOUCH;
                inputInfo[2]=NOTOUCH;
                keyFlags &= ~0b1000;
            }

            if ((keyFlags!=lastFlag) && !(kHeld & KEY_TOUCH)) {
                inputInfo[0]=keyFlags;
                lastFlag=keyFlags;
                status=send(PCServerSocket, inputInfo, sizeof (inputInfo),0);
                if (status!=sizeof(inputInfo)) {
                    failExit("send: %d %s\n", errno, strerror(errno));
                }
            }
            if (kHeld & KEY_TOUCH) {

                inputInfo[0]=keyFlags;
                lastFlag=keyFlags;
                //Read the touch screen coordinates
                hidTouchRead(&touch);

                inputInfo[1]=touch.px;
                inputInfo[2]=touch.py;
                if (keyFlags & 0b1000) {
                    status=send(PCServerSocket, inputInfo, sizeof (inputInfo),0);
                    if (status!=sizeof(inputInfo)) {
                        printf("JE T'EMMERDE\n");
                        failExit("send: %d %s\n", errno, strerror(errno));

                    }
                }
            }

        }

            // Flush and swap framebuffers
            gfxFlushBuffers();
            gfxSwapBuffers();
        }
    if (DSServerSocket > 0) close(DSServerSocket);
    if (PCClientSocket > 0) close(PCClientSocket);
    if (PCServerSocket > 0) close(PCServerSocket);
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
