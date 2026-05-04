#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "configreader.h"

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

ProcTableEntry gProcTable[MAX_NUM_CHILDREN];
ThreadParams * gpThreadParams = NULL;

void StartChildren();
void *SignalThread( void *ptr );
void StartProc(ProcTableEntry * lpProcTableEntry);
void TermChildren();
void RestartChild(int lnPid);

void SigHandler(int sig, siginfo_t *info, void *context)
{
    gpThreadParams = (ThreadParams*) malloc(sizeof(ThreadParams));
    gpThreadParams->lnSignal = sig;
    gpThreadParams->lpSigInfo = info;
    context = context;

    pthread_t thread1;
    int iret1 = pthread_create(&thread1, NULL, SignalThread, (void*) gpThreadParams);

    if (iret1 != 0)
    {
        printf("Unable to create thread. Exiting");
        exit(-1);
    }
}

int main()
{
    SystemConfig lsSystemConfig;

    GetSystemConfig(&lsSystemConfig);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = SigHandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);

    strncpy(gProcTable[0].lanProcName, lsSystemConfig.lanChild1Proc, 1024);
    gProcTable[0].lnPid = -1;

    strncpy(gProcTable[1].lanProcName, lsSystemConfig.lanChild2Proc, 1024);
    gProcTable[1].lnPid = -1;

    strncpy(gProcTable[2].lanProcName, lsSystemConfig.lanChild3Proc, 1024);
    gProcTable[2].lnPid = -1;

    StartChildren();

    for (int i = 1;; i++)
    {
        printf("%d : *** PARENT: %d\n", i, getpid());
        sleep(3);
    }
    return 0;
}

void StartChildren()
{
    for (int i = 0; i < MAX_NUM_CHILDREN; i++)
    {
        StartProc(&gProcTable[i]);
    }
}

void StartProc(ProcTableEntry * lpProcTableEntry)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(-1);
    }
    else if (pid == 0)
    {
        char *nl = strchr(lpProcTableEntry->lanProcName, '\n');
        if (nl) *nl = '\0';

        execl(lpProcTableEntry->lanProcName,
              lpProcTableEntry->lanProcName,
              NULL);

        perror("execl failed");
        exit(-1);
    }
    else
    {
        lpProcTableEntry->lnPid = pid;
        printf("Started child %s with PID %d\n", lpProcTableEntry->lanProcName, pid);
    }
}

void RestartChild(int lnPid)
{
    for (int i = 0; i < MAX_NUM_CHILDREN; i++)
    {
        if (gProcTable[i].lnPid == lnPid)
        {
            printf("Restarting child %s (old PID %d)\n", gProcTable[i].lanProcName, lnPid);
            StartProc(&gProcTable[i]);
            return;
        }
    }
    printf("RestartChild: PID %d not found in process table\n", lnPid);
}

void TermChildren()
{
    for (int i = 0; i < MAX_NUM_CHILDREN; i++)
    {
        if (gProcTable[i].lnPid > 0)
        {
            printf("Terminating child %s (PID %d)\n", gProcTable[i].lanProcName, gProcTable[i].lnPid);
            kill(gProcTable[i].lnPid, SIGTERM);
            gProcTable[i].lnPid = -1;
        }
    }
}

void *SignalThread( void *ptr )
{
    ThreadParams * lpThreadParams = (ThreadParams*) ptr;

    switch(lpThreadParams->lnSignal)
    {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
        case SIGABRT:
        printf("****** Process %d receivd SIGINT (%d) from process %d\n",
               getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);
        TermChildren();
        exit(-10);
        break;

        case SIGCHLD:
        printf("****** Process %d receivd SIGCHLD (%d) from process %d\n",
               getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);
        RestartChild(lpThreadParams->lpSigInfo->si_pid);
        break;

        default:
        printf("Process %d receivd signal %d from process %d\n",
               getpid(), lpThreadParams->lnSignal, lpThreadParams->lpSigInfo->si_pid);
    }

    free(lpThreadParams);
    return NULL;
}
