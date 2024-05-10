#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILDREN 32
#define INT_MAX 15000000

int main (int argc, char *argv[]) 
{
    int pid = 0;
    for (int i = 0; i < NCHILDREN; i++) 
    {
        pid = fork();
        if (pid < 0) 
        {
            printf("fork failed\n");
            exit(-1);
        }
        
        else if (pid == 0) 
        {
            int temp = 0;
            for (int j = 0; j < INT_MAX; j++) {
                temp++;
            }
            exit(0);
        } 
        
        else 
        {
            pid = wait(0);
        }
    }
    exit(0);
}