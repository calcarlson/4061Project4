// Test Machine : CSELAB machine name
// Date : 04/10/20
// Name : Conrad Carlson , Nam Trinh, Jessica Cunningham
// x500 : Carl5362 , Namht1999, Cunni536

#include "server.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>

//  -------------------- MAIN GOAL --------------------  //
// The aim of the server is to maintain a global array   //
// which stores the word count for each letter (how many //
// words start with ‘a’, how many words start with       //
// ‘b’, etc.). This array will be updated by its threads,//
// where each thread receives some paths to text files   //
// from a client process. The threads will modify the    //
// array to reflect the number of words starting with a  //
// letter in those files. When all the client processes  //
// have finished sending up requests, the server threads //
// send the final counts of the global array to          //
// the clients.                                          //

//  ---------------- PRINT STATEMENTS -----------------  //
// 1) Server Start                                      -XX
// 2) Each file path received (PID and path)            -XX
// 3) Each Acknowledge to client "ACK" (PID and path)   -XX
// 4) Each "END" received from Client (PID and path)     //
// 5) After receiving “END” from client (Thread id)     -XX
// 6) Server Ends                                       -XX

// (Process ID = (0,1,...n-1) n=total number of threads) //

// Global array structure to store word counts of each letter
struct wordCount {
int counter[26];
pthread_mutex_t mutexCounter;
int updatedCount;
pthread_mutex_t mutexUpdated;
pthread_cond_t condUpdated;
};
// Global message buffer to store type and content of messages
struct msessageBuffer{
    long messageType;
    char messageText[512];
} message;
// Global structure to store thread id information
struct threadCount{
pthread_t threads;
int clientID;
struct wordCount* shared; 
};

// function to open a file and read in the first letter of
// each word line by line into an array
void readFile(int clientID, char *fileName, int *output)
{
    FILE *fd = fopen(fileName, "r");
    printf("Opening received file at %s", fileName);
    while (true){
        char *fileLine = NULL;
        unsigned long n = 0;
        if (getline(&fileLine, &n, fd) == -1)
        {
            printf("getline");
            break;
        }
        output[toupper(fileLine[0]) - 'A']++;
    }
    fclose(fd);
}

// Process to create threads and assign IDs for use in 
// the client program also translates the array to an output
void* createThread(void* arg){
    struct threadCount* process =(struct threadCount*) arg;
    struct wordCount* shared = process->shared;
    int clientID = process->clientID;
    key_t key = ftok("queue", 8);
    int msgq = msgget(key, IPC_CREAT | 0600);
    while(true){
        char *fileName = message.messageText;
        if(strcmp(fileName, "END")==0){
            break;
        }
        printf("Received path %s", fileName);
        int output[26];
        memset(output,0, 26);
        readFile(clientID,fileName,output);
//MUTEX LOCKS HERE MAYBE?
// Iterate through the read file and copy info into wordcount struct
        for(int i=0;i<26;i++){
            shared->counter[i]+=output[i];
        }
        printf("ACK %s",fileName);
        message.messageType=1025+clientID;
        strcpy(message.messageText,"ACK");
        msgsnd(msgq,&message,1024,0);

    }
    printf("received END");
//decrement the count and broadcast thread status if last thread 
    shared->updatedCount--;
    if(shared->updatedCount == 0){
        printf("last Thread");
        pthread_cond_broadcast(&shared->condUpdated);
    }
    else{
        printf("Waiting on shared->condUpdated");
        while(shared->updatedCount!=0){
            pthread_cond_wait(&shared->condUpdated,&shared->mutexUpdated);
        }
    }
    printf("Output:");
    char output[1024];
    char* current = output;
//Convert the array from readFile into an output for Client in the queue 
    for(int i=0;i<26;i++){
        current += sprintf(current,i==25?"%d":"%d#",shared->counter[i]);
    }
    message.messageType=1025+clientID;
    strcpy(message.messageText,output);
    msgsnd(msgq,&message,1024,0);
    return NULL;
}
int main(int argc, char **argv)
{
    printf(" -Server Started- \n");
// Input argument validation
    if (argc > 2)
    {
        printf("Too many arguments, You must enter one argument \n");
        exit(1);
    }
    else if (argc == 1)
    {
        printf("Too few arguments, You must enter one argument \n");
        exit(1);
    }

printf("testing threadCounter");
    int threadCounter = argc;
// Creating an instant of thread struct for the thread counter
printf("testing structs");
    struct threadCount threads[threadCounter];
    struct wordCount shared;
// Set the array initialally to all zeroes before counting.
    memset(shared.counter, 0, sizeof(shared.counter));
// Create the number of threads defined by threadCounter 
    shared.updatedCount = threadCounter;
// Initialize the mutex objects required for server sync
printf("testing threads");
    pthread_mutex_init(&shared.mutexCounter,NULL);
    pthread_mutex_init(&shared.mutexUpdated,NULL);
    pthread_cond_init(&shared.condUpdated,NULL);
// Create threads based on threadCounter(Argument passed to Server)   
    for(int i=0;i<threadCounter;i++){
        threads[i].clientID=i;
        threads[i].shared = &shared;
        pthread_create(&threads[i].threads,NULL,&createThread,&threads[i]);
    }
// Wait for the created threads to be completed by Client
    for(int i=0;i<threadCounter;i++){
        pthread_join(threads[i].threads,NULL);
    }

// End the created threads
    pthread_mutex_destroy(&shared.mutexCounter);
    pthread_mutex_destroy(&shared.mutexUpdated);
    pthread_cond_destroy(&shared.condUpdated);

    printf(" -Server Ended- \n");
    
}
