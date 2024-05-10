// Kristy Nguyen (5006243601), Section 1003, Project 2
// A C program that provides a count of the perfect / abundant / 
// deficient numbers between 2 and some limit.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

#define THREAD_COUNT 4

// Global variables
pthread_mutex_t     myLock;                 // mutex variable
int                 LIMIT           = 0;
int                 counter         = 2;
int                 perfectCount    = 0;
int                 abundantCount   = 0;
int                 deficientCount  = 0;

void* threadFunction()
{
    int n = 0;
    int sum = 0;

    pthread_mutex_lock(&myLock);
    n = counter;
    counter++;
    pthread_mutex_unlock(&myLock);
    
    while (n <= LIMIT)
    {
        // Find divisors and sum them up
        sum = 1;
        for (int i = 2; i*i <= n; ++i)
        {
            if (n % i == 0)
            {
                sum += i;
                sum += n/i;
            }

            if (i * i == n)
                sum -= i;
        }

        if (sum == n)
        {
            pthread_mutex_lock(&myLock);
            perfectCount++;
            pthread_mutex_unlock(&myLock);
        }
        else if (sum > n)
        {
            pthread_mutex_lock(&myLock);
            abundantCount++;
            pthread_mutex_unlock(&myLock);
        }
        else if (sum < n)
        {
            pthread_mutex_lock(&myLock);
            deficientCount++;
            pthread_mutex_unlock(&myLock);
        }
        
        pthread_mutex_lock(&myLock);
        n = counter;
        counter++;
        pthread_mutex_unlock(&myLock);
    }

    return NULL;
}

int main (int argc, char* argv[])
{
    pthread_t   threadArray[THREAD_COUNT];
    int numOfThreads = THREAD_COUNT;

    // Command line argument checks
    if (argc == 1)
    {
        printf("Usage: ./numCount <s|p> <limitValue>\n");
        exit(0);
    }
    else if (argc != 3)
    {
        printf("Error, invalid command line arguments.\n");
        exit(0);
    }

    // Invalid command line arguments
    if (strcmp(argv[1], "s") && strcmp(argv[1], "p"))
    {
        printf("Error, invalid command line arguments.\n");
        exit(0);
    }
    
    // Invalid limit value (not integer)
    for (int i = 0; i < strlen(argv[2]); i++)
    {
        if (argv[2][i] < '0' || argv[2][i] > '9')
        // if (isdigit(argv[2][i]) == 0)
        {
            printf("Error, invalid limit value.\n");
            exit(0);
        }
    }

    // Limit is < 100
    LIMIT = atoi(argv[2]);
    if (LIMIT < 100)
    {
        printf("Error, limit must be > 100.\n");
        exit(0);
    }
    
    // Initialize myLock mutex
    pthread_mutex_init(&myLock, NULL);

    printf("Count of Perfect, Abundant, and Deficient numbers from 1 to %d\n", LIMIT);
    printf("Please wait. Running...\n\n");

    if (strcmp(argv[1], "s") == 0)
        numOfThreads = 1;
    else if (strcmp(argv[1], "p") == 0)
        numOfThreads = THREAD_COUNT;

    // Start up threads...
    for (int i = 0; i < numOfThreads; i++)
        pthread_create(&threadArray[i], NULL, &threadFunction, NULL);

    // Wait for threads to finish...
    for (int i = 0; i < numOfThreads; i++)
        pthread_join(threadArray[i], NULL);

    // Finished running threads...
    printf("Count of Perfect Numbers = %d\n", perfectCount);
    printf("Count of Abundant Numbers = %d\n", abundantCount);
    printf("Count of Deficient Numbers = %d\n", deficientCount);
    printf("Total = %d\n\n", perfectCount + abundantCount+ deficientCount);
    return 0;
}