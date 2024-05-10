// Kristy Nguyen (5006243601), Section 1003, Project 3
// Semaphore Project

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>      // for errno

#include <string.h>

const int DATACNT_MIN = 5;
const int DATACNT_MAX = 20;

const int BUFFER_MIN = 5;
const int BUFFER_MAX = 20;

#define   THREAD_CNT   5
// const int THREAD_CNT = 5;

int maxSize    = 0;    // maxValue
int bufferSize = 0;    // sizeValue

sem_t           prodSemaphore;                  // producer semaphore
sem_t           consSemaphore;                  // consumer semaphore
pthread_mutex_t myLock;                         // mutex lock
int             threadID_array[THREAD_CNT];     // thread id array
int             *buffer;                        // buffer
int             bufferCount = 0;                // buffer count

int checkCommandLineArgs (int argc, char* argv[], int* maxValue, int* bufferSize)
{
    *maxValue = atoi(argv[2]);      // 3rd argument
    *bufferSize = atoi(argv[4]);    // 5th argument

    // Command line must have 5 arguments
    if (argc != 5)
    {
        printf("Usage: ./prod-cons -i <maxValue> -b <sizeValue>\n");
        return -1;
    }

    // 2nd argument must be "-i"
    if (strcmp(argv[1], "-i") != 0)
    {
        printf("Error, invalid command line arguments.\n");
        return -1;
    }

    // maxValue must be between DATACNT_MIN and DATACNT_MAX
    if (*maxValue < DATACNT_MIN || *maxValue > DATACNT_MAX)
    {
        printf("Error, <maxValue> must be between 5 and 20.\n");
        return -1;
    }

    // 4th argument must be "-b"
    if (strcmp(argv[3], "-b") != 0)
    {
        printf("Error, invalid command line arguments.\n");
        return -1;
    }

    // sizeValue must be bewteen BUFFER_MIN and BUFFER_MAX
    if (*bufferSize < BUFFER_MIN || *bufferSize > BUFFER_MAX)
    {
        printf("Error, <sizeValue> must be between 5 and 20.\n");
        return -1;
    }


    return 1;
}

void *prodFunction (void* threadID)
{
    int *printID = (int *) threadID;
    int dataItem = 0;
    
    for (int i = 0; i < maxSize; i++)
    {
        usleep(rand() % 2000);
        dataItem = rand();

        sem_wait(&prodSemaphore);
        
        pthread_mutex_lock(&myLock);
        buffer[bufferCount % bufferSize] = dataItem;
        bufferCount = (bufferCount + 1) % bufferSize;
        pthread_mutex_unlock(&myLock);

        printf("\033[0;31mProducer %d: Insert Item %d at %d\033[0m\n", *printID, dataItem, bufferCount);

        sem_post(&consSemaphore);
    }

    return NULL;
}

void *consFunction (void* threadID)
{
    int *printID = (int *) threadID;
    int removedItem = 0;
    
    for (int i = 0; i < maxSize; i++)
    {
        sem_wait(&consSemaphore);
        
        pthread_mutex_lock(&myLock);
        removedItem = buffer[bufferCount % bufferSize];
        bufferCount = (bufferCount + 1) % bufferSize;
        pthread_mutex_unlock(&myLock);
        
        printf("\033[0;92mConsumer %d: Remove Item %d from %d\033[0m\n", *printID, removedItem, bufferCount);

        usleep(rand() % 2000);

        sem_post(&prodSemaphore);
    }
    
    return NULL;
}

int main (int argc, char* argv[])
{
    // Producer and consumer thread arrays
    pthread_t *prodThreads;
    pthread_t *consThreads;

    // Read and validate command line arguments
    if (checkCommandLineArgs(argc, argv, &maxSize, &bufferSize) < 0)
        exit(1);
    
    // Initialize mutex and semaphores
    pthread_mutex_init(&myLock, NULL);
    
    sem_init(&prodSemaphore, 0, bufferSize);
    sem_init(&consSemaphore, 0, 0);
    
    // Create array for thread IDs
    for (int i = 0; i < THREAD_CNT; i++)
        threadID_array[i] = i;

    // Dynamically allocate arrays
    buffer      = (int *)       malloc(bufferSize * sizeof(int));                      // buffer
    prodThreads = (pthread_t *) malloc(THREAD_CNT * sizeof(pthread_t));     // producer thread array
    consThreads = (pthread_t *) malloc(THREAD_CNT * sizeof(pthread_t));     // consumer thread array

    // Display header
    printf("\033[0;4mCS 370 Producer-Consumer Project\033[0m\n\n");

    // Create THREAD_CNT producer and consumer threads
    for (int i = 0; i < THREAD_CNT; i++)
    {
        int *tid = malloc(sizeof(*tid));
        *tid = threadID_array[i];
        pthread_create(&prodThreads[i], NULL, &prodFunction, (void *) tid);
        pthread_create(&consThreads[i], NULL, &consFunction, (void *) tid);
        free (tid);
    }

    // Wait for producer and consumer threads to finish
    for (int i = 0; i < THREAD_CNT; i++)
    {
        pthread_join(prodThreads[i], NULL);
        pthread_join(consThreads[i], NULL);
    }

    free (buffer);
    free (prodThreads);
    free (consThreads);

    printf("\n\033[0;4mProject Done\033[0m\n\n");

    return 0;
}