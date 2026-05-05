/*
    Author: Joe Waclawski
    Description: This program will read a configuration file to get a list
    of child processes it can kill, and their port numbers. The user will 
    enter the port number to send a udp message to the child, at which point
    the child will exit.
*/

#include<stdio.h>               //printf 
#include<string.h>              //memset
#include<stdlib.h>              //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>              //close
#include "configreader.h"

#define BUFLEN 512              //Max length of buffer

// simple routine to print an error and exit
void PrintErrorAndExit(char *lanErrorString){
    perror(lanErrorString);
    exit(1);
}

int main(void){
    // get configuration data so we know what children we can kill
    SystemConfig lsSystemConfig;

    // Read configuration
    GetSystemConfig(&lsSystemConfig); 

    // inf loop for selection
    while(1){
        // Show the user a menu
        printf("Which child do you want to kill?\n");
        printf("1) %s (port %d)\n", lsSystemConfig.lanChild1Proc, lsSystemConfig.lnChild1Port);
        printf("2) %s (port %d)\n", lsSystemConfig.lanChild2Proc, lsSystemConfig.lnChild2Port);
        printf("3) %s (port %d)\n", lsSystemConfig.lanChild3Proc, lsSystemConfig.lnChild3Port);
        printf("0) Quit\n");

        // Get user choice
        int choice;
        scanf("%d", &choice);

        if(choice == 0){
            printf("Exiting ProcKiller\n");
            break;
        }
        
        // Figure out which port to send it to
        int targetPort = -1;
        if(choice == 1){
            targetPort = lsSystemConfig.lnChild1Port;
        }else if(choice == 2){
            targetPort = lsSystemConfig.lnChild2Port;
        }else if(choice == 3){
            targetPort = lsSystemConfig.lnChild3Port;
        }else{
            printf("Invalid choice, try again\n");
            continue;
        }

        // Create the UDP socket
        int lnSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (lnSocket == -1){
            PrintErrorAndExit("socket failed");
        }
        
        // Build the destination and send
        struct sockaddr_in lsDest;
        memset(&lsDest, 0, sizeof(lsDest));
        lsDest.sin_family = AF_INET;
        lsDest.sin_port = htons(targetPort);
        inet_aton("127.0.0.1", &lsDest.sin_addr);

        char laBuf[] = "KILL";
        sendto(lnSocket, laBuf, strlen(laBuf), 0,
            (struct sockaddr*)&lsDest, sizeof(lsDest));

        printf("Sent kill signal to port %d\n", targetPort);
        close(lnSocket);
    }
    

    return 0;
}

