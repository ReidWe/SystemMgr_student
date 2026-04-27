/*
    Author: Joe Waclawski
    Shell System Manager that students can use
    in their CSE384 Final Projecgt
*/
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "configreader.h"

// structure to hold the parameters that are sent to the signal handling thread
typedef struct tp
{
    int lnSignal;
    siginfo_t * lpSigInfo;
} ThreadParams;

typedef struct pt
{
    char lanProcName[1024];
    int lnPid;
} ProcTableEntry;

    
// TODO
// create the process empty table
// This must be global so all functions can easily access it. 
// To create the process table, you will need an array of 3 ProcTableEntry structure. 

// make thread params pointer global so that it persists in the thread
ThreadParams * gpThreadParams = NULL;

// prototypesdd
void StartChildren();
void *SignalThread( void *ptr );
void StartProc(ProcTableEntry * lpProcTableEntry);
void TermChildren();

// our signal handler
// NOTHING TODO HERE
void SigHandler(int sig, siginfo_t *info, void *context)
{
    // need to dynamically allocate memory, which must exist AFTER
    // signal handler exists
    // Thread MUST delete memory to prevent memory leak
    gpThreadParams = (ThreadParams*) malloc(sizeof(ThreadParams));
    gpThreadParams->lnSignal = sig;
    gpThreadParams->lpSigInfo = info;
    // we don't uset the context. it is for advanced signal handling, but
    // we need to do something with it to get rid of the unused parameter compiler warning
    context = context;    

    // create a thread to handle the signal   
    pthread_t thread1;    
    int iret1 = pthread_create( &thread1, NULL, SignalThread, (void*) gpThreadParams);
    
    if (iret1 !=0)
    {
	    printf("Unable to create thread. Exiting");
	    exit(-1); // exiting ensures any threads that did start will be terminated
    }
}


int main()
{
    // place to store our system config
    SystemConfig lsSystemConfig;
    
    // Read configuration
    // This will define information about the child processes for use
    // by all applications in the Final Project. 
    GetSystemConfig(&lsSystemConfig); 
    
    
    
    // TODO
    // install the signal handlers
    // use the sigaction library function instead of signal to get more information
    // about the process that sent the signal
    // the generic signal handler has already been defined.

    // TODO
    // Initialize the process table with information from the configuration
    // Note that the lnPid member will not initially defined.
    
    // start children
    StartChildren();
    
    // do nothing waiting to be terminated 
    for(int i=1;;i++)
    {    //Infinite loop
        printf("%d : *** PARENT: %d\n",i, getpid());
        sleep(3);  // Delay
    }
    return 0;
}
  

void StartChildren() 
{  
	// TODO
	// complete this function by starting children defined in process table
	// don't forget update the pid in the table
}


void RestartChild(int lnPid)
{
	// TODO
	// complete this function by restarting the terminated child represented
	// by the Pid that was passed in
	// HINT, search the Process table for the Pid, and replace it
	// with the newly created child process
}

void TermChildren()
{
	// TODO
	// terminate all child processed found in the Process table
}

// Signal thread that handles all signals for which this 
// application registered
// NOTHING TODO HERE
void *SignalThread( void *ptr )
{
    // cast our input argument to the correct type that we are expecting
    ThreadParams * lpThreadParams = (ThreadParams*) ptr;
    
    switch(lpThreadParams->lnSignal)
    {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
        case SIGABRT:
        printf("****** Process %d receivd SIGINT (%d) from process %d\n", getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);
        TermChildren();
        exit(-10);
        break;
        
        case SIGCHLD:
        printf("****** Process %d receivd SIGCHLD (%d) from process %d\n", getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);
        
        // restart the child that just died
        RestartChild(lpThreadParams->lpSigInfo->si_pid);
        break;
        
        default:
        printf("Process %d receivd signal %d from process %d\n", getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);        
    }
    
    // per comment in Signal Handler, the memory must be deleted when 
    // we are done with it
    free(lpThreadParams);
    
    return NULL;
}
