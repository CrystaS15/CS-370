#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    // Define variables for uptime and fork
    int start_time = uptime();
    int end_time = 0;
    int pid = fork();
    int real_time;

    if (argc <= 1)
    {
        printf("Usage: time <command>\n");
    }

    // Child process
    if (pid == 0)
    {
        // Execute command user provided command line arguments
        exec(argv[1], argv+1);
        exit(1);
    }

    // Parent process
    else if (pid > 0)
    {
        wait(0);

        // Get current time with uptime()
        end_time = uptime();

        real_time = end_time - start_time;

        printf("\nReal-time in ticks: %d\n", real_time);

        exit(1);
    }

    // Error check
    else
    {
        printf("Unsuccessful fork operation!\n");
        exit(1);
    }

    exit (0);
}
// argc == 1 error 
// start time = uptime()
// end time = 0
// pid = fork();

// process == 0 aka child process 