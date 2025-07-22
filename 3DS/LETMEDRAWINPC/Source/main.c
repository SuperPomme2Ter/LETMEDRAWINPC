// /*
// 	Touch Screen example made by Aurelio Mannara for libctru
// 	Please refer to https://github.com/devkitPro/libctru/blob/master/libctru/include/3ds/services/hid.h for more information
// 	This code was modified for the last time on: 12/13/2014 2:30 UTC+1
//
// 	This wouldn't be possible without the amazing work done by:
// 	-Smealum
// 	-fincs
// 	-WinterMute
// 	-yellows8
// 	-plutoo
// 	-mtheall
// 	-Many others who worked on 3DS and I'm surely forgetting about
// */
//
// #include <3ds.h>
// #include <stdio.h>
//
// struct vector {
// 	int x;
// 	int y;
// };
//
// int main(int argc, char **argv)
// {
// 	gfxInitDefault();
//
// 	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
// 	consoleInit(GFX_TOP, NULL);
//
// 	printf("\x1b[0;0HPress Start to exit.");
// 	printf("\x1b[1;0HTouch Screen position:");
// 	struct vector lastScreenPos;
// 	lastScreenPos.x = 0;
// 	lastScreenPos.y = 0;
//
//
// 	// Main loop
// 	while (aptMainLoop())
// 	{
// 		//Scan all the inputs. This should be done once for each frame
// 		hidScanInput();
//
// 		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
// 		u32 kDown = hidKeysDown();
//
//
// 		if (kDown & KEY_START) break; // break in order to return to hbmenu
//
// 		touchPosition touch;
// 		struct vector dir;
// 		dir.x = 0;
// 		dir.y = 0;
// 		//Read the touch screen coordinates
// 		hidTouchRead(&touch);
// 		dir.x=lastScreenPos.x-touch.px;
// 		dir.y=lastScreenPos.y-touch.py;
// 		//Print the touch screen coordinates
// 		printf("\x1b[2;0H%03d; %03d", touch.px, touch.py);
// 		printf("\x1b[3;0H%03d; %03d", dir.x, dir.y);
// 		lastScreenPos.x = touch.px;
// 		lastScreenPos.y = touch.py;
//
//
// 		// Flush and swap framebuffers
// 		gfxFlushBuffers();
// 		gfxSwapBuffers();
//
// 		//Wait for VBlank
// 		gspWaitForVBlank();
// 	}
// 	// Exit services
// 	gfxExit();
// 	return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <3ds.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

const static char test[] = "le client est connecte";

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

    u32 clientlen;
    struct sockaddr_in client;
    struct sockaddr_in server;
    char temp[1026];
    //static int hits=0;
    //struct sockaddr_in PCServer;

    gfxInitDefault();

    // register gfxExit to be run when app quits
    // this can help simplify error handling
    atexit(gfxExit);

    consoleInit(GFX_TOP, NULL);

    printf ("\nlibctru sockets demo\n");

    // allocate buffer for SOC service
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

    if(SOC_buffer == NULL) {
        failExit("memalign: failed to allocate\n");
    }

    // Now intialise soc:u service
    if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
        failExit("socInit: 0x%08X\n", (unsigned int)ret);
    }

    // register socShutdown to run at exit
    // atexit functions execute in reverse order so this runs before gfxExit
    atexit(socShutdown);

    // libctru provides BSD sockets so most code from here is standard
    clientlen = sizeof(client);

    sock = socket (AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        failExit("socket: %d %s\n", errno, strerror(errno));
    }

    memset (&server, 0, sizeof (server));
    memset (&client, 0, sizeof (client));

    server.sin_family = AF_INET;
    server.sin_port = htons (8000);
    server.sin_addr.s_addr = gethostid();

    printf("3DS IP Adress %d in %s\n", server.sin_port, inet_ntoa(server.sin_addr));
    printf("Now listening\n");

    if ( (ret = bind (sock, (struct sockaddr *) &server, sizeof (server))) ) {
        close(sock);
        failExit("bind: %d %s\n", errno, strerror(errno));
    }
    // Set socket non blocking so we can still read input to exit
    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

    if ( (ret = listen( sock, 5)) ) {
        failExit("listen: %d %s\n", errno, strerror(errno));
    }



    while (aptMainLoop()) {
        gspWaitForVBlank();
        hidScanInput();

        csock = accept (sock, (struct sockaddr *) &client, &clientlen);



        if (csock<0) {
            if(errno != EAGAIN) {
                failExit("accept: %d %s\n", errno, strerror(errno));
            }
        } else {
            // set client socket to blocking to simplify sending data back
            fcntl(csock, F_SETFL, fcntl(csock, F_GETFL, 0) & ~O_NONBLOCK);
            printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
            memset (temp, 0, 1026);

            //ret = recv (csock, temp, 1024, 0);

            printf("Sending test message\n");
            send(csock, test, strlen(test),0);

            close (csock);
            csock = -1;

        }

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break;
    }

    close(sock);

    return 0;
}

//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...) {
//---------------------------------------------------------------------------------

    if(sock>0) close(sock);
    if(csock>0) close(csock);

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