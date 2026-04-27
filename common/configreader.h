#include <stdlib.h>

#define MAXLINE_LEN 1024
#define MAX_NUM_CHILDREN 3


typedef struct sysconfig
{
    char lanServerAddress[1024];
    char lanChild1Proc[1024];
    int lnChild1Port;
    char lanChild2Proc[1024];
    int lnChild2Port;
    char lanChild3Proc[1024];
    int lnChild3Port;
    
} SystemConfig;

int GetSystemConfig(SystemConfig * sysConfig);