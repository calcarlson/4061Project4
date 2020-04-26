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

// Process to create threads and assign IDs for use in 
// the client program
void* createThread(void* arg){
    struct threadCount* process =(struct threadCount*) arg;
    struct wordCount* shared = process->shared;
    int clientID = process->clientID;

    key_t key = ftok("queue", 8);
    int msgq = msgget(key, IPC_CREAT | 0600);
    printf("path %d", fileName);

    while(true){
        char *fileName = message.messageText;
        if(strcmp(filename, "END")==0){
            break;
        }
        printf("Received path %s", fileName);
    }
    printf("received END");
}

// function to open a file and read in the first letter of 
// each word line by line into the word array
void readFile(int clientID, char* fileName, int* letterOfLine){
    FILE* fd = fopen(fileName, "r");
    printf("Opening received file at %d",fileName);
    while(true){
        char* fileLine = NULL;
        unsigned long n =0;
        if(getline(&line, &n, fd) == -1){
            printf("getline");
            break;
        }
        letterOfLine[toupper(fileLine[0]) - 'A']++;
    }
    fclose(fd);
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
    int threadCounter = argc;

// Creating an instant of thread struct for the thread counter
    struct threadCount threads[threadCounter];
    struct wordCount words;

    for(int i=0;i<threadCounter;i++){
        threads[i].clientID=i;
        threads[i].shared = &shared;
        pthread_create(&threads[i].threads,NULL,&)
    }

//set the array initialally to all zeroes before counting.
    memset(words.count, 0, n*sizeof[words.count]);
//create the number of threads defined by the threadcounter    
    words.updatedCount=threadCounter;
    printf(" -Server Ended- \n");
}