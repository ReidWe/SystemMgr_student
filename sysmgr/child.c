#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include<arpa/inet.h> 
#include<sys/socket.h>
#include "configreader.h"

#define BUFLEN 1024

void *DeathThread( void *ptr );

typedef struct sp
{
    char lanServerAddress[1024];
    int lnPort;
} SocketData;

// simple routine to print an error and exit
void PrintErrorAndExit(char *lanErrorString)
{
    perror(lanErrorString);
    exit(1);
}

int main(int argc, char * argv[])
{

    int lnPid;
    int count = 0;

    // get rid of "unused variable" warning
    argc = argc;
    // get configuration data so we know what port we are supposed to be
    SystemConfig lsSystemConfig;
    // Read configuration
    GetSystemConfig(&lsSystemConfig); 
    
    // TODO
    // read the command line parameters to see what child I am
    // start the DeathThread, passing it the port associated with this child

    while (1)
    {    //Infinite loop
        printf("%d : ****** %s : %d\n",++count, argv[0], lnPid);
        sleep(3);  // Delay for 1 second
    }
    return 0;
}

void *DeathThread( void *ptr )
{
    // TODO
    // read the port sent in through the *ptr variable.
    // Set up a socket and listen on that port until we get a message indicating that this child should terminate
   


    
    exit(0);
}
  

