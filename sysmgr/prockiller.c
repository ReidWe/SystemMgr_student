/*
    Author: Joe Waclawski
    Description: This program will read a configuration file to get a list
    of child processes it can kill, and their port numbers. The user will 
    enter the port number to send a udp message to the child, at which point
    the child will exit.
*/

#include<stdio.h>    //printf 
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h> //close
#include "configreader.h"

#define BUFLEN 512    //Max length of buffer

// simple routine to print an error and exit
void PrintErrorAndExit(char *lanErrorString)
{
    perror(lanErrorString);
    exit(1);
}

int main(void)
{
    
   // get configuration data so we know what children we can kill
    SystemConfig lsSystemConfig;
    // Read configuration
    GetSystemConfig(&lsSystemConfig); 

    // TODO
    // Using the System Configuration informaton to see what port each child is listening on 
    // set up a socket and send that child a message which will signal it to terminate
    
    return 0;
}

