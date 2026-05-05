#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "configreader.h"

#define BUFLEN 1024

// struct to pass socket info to the death thread
typedef struct sp{
    char lanServerAddress[1024];
    int lnPort;
} SocketData;

// forward declaration so main can see DeathThread
void *DeathThread( void *ptr );

// simple error helper
void PrintErrorAndExit(char *lanErrorString){
    perror(lanErrorString);
    exit(1);
}

int main(int argc, char * argv[]){
    (void)argc; // not using argc, this suppresses the compiler warning

    int lnPid = getpid();
    int count = 0;
    int lnPort = -1;

    // read the config file to find out which port this child listens on
    SystemConfig lsSystemConfig;
    GetSystemConfig(&lsSystemConfig);

    // strip the directory path from argv[0]
    char *progName = strrchr(argv[0], '/');
    progName = progName ? progName + 1 : argv[0];

    // figure out which port belongs to us based on our program name
    if (strcmp(progName, "child1") == 0){
        lnPort = lsSystemConfig.lnChild1Port;
    } else if (strcmp(progName, "child2") == 0){
        lnPort = lsSystemConfig.lnChild2Port;
    } else if (strcmp(progName, "child3") == 0){
        lnPort = lsSystemConfig.lnChild3Port;
    }

    // if we didn't match any known name, something is wrong
    if (lnPort == -1) {
        fprintf(stderr, "ERROR: Unknown child name '%s', cannot determine port\n", progName);
        exit(1);
    }

    // print startup message so sysmgr output shows this child came alive
    printf("****** Child %s started with PID %d on port %d\n", progName, lnPid, lnPort);

    // allocate struct for thread 
    SocketData *lpSocketData = malloc(sizeof(SocketData));
    if (lpSocketData == NULL){
        PrintErrorAndExit("malloc failed");
    } 
    lpSocketData->lnPort = lnPort;

    // spin up the death thread
    pthread_t deathThread;
    if (pthread_create(&deathThread, NULL, DeathThread, (void*) lpSocketData) != 0)
        PrintErrorAndExit("pthread_create failed");

    pthread_detach(deathThread);

    // main loop
    while (1){
        printf("%d : ****** %s : %d\n", ++count, progName, lnPid);
        sleep(3);
    }
    return 0;
}

void *DeathThread( void *ptr ){
    // pull the port out of the struct then free it
    SocketData *lpSocketData = (SocketData*) ptr;
    int lnPort = lpSocketData->lnPort;
    free(lpSocketData);

    // create a UDP socket to listen for incoming kill messages
    int lnSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lnSocket == -1) PrintErrorAndExit("socket failed");

    // bind to our assigned port on any network interface
    struct sockaddr_in lsAddr;
    memset(&lsAddr, 0, sizeof(lsAddr));
    lsAddr.sin_family      = AF_INET;
    lsAddr.sin_port        = htons(lnPort);
    lsAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(lnSocket, (struct sockaddr*)&lsAddr, sizeof(lsAddr)) == -1)
        PrintErrorAndExit("bind failed");

    // block here waiting for any UDP message
    char laBuf[BUFLEN];
    struct sockaddr_in lsSender;
    socklen_t lnLen = sizeof(lsSender);
    recvfrom(lnSocket, laBuf, BUFLEN, 0, (struct sockaddr*)&lsSender, &lnLen);

    // got the kill signal
    printf("****** Child PID %d received kill message on port %d, exiting\n", getpid(), lnPort);
    close(lnSocket);
    exit(0);
}