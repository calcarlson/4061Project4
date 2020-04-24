// Test Machine : CSELAB machine name
// Date : 04/10/20
// Name : Conrad Carlson , Nam Trinh, Jessica Cunningham
// x500 : Carl5362 , Namht1999, Cunni536

#include "server.h"
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

//  -------------------- MAIN GOAL -------------------- 
// The aim of the server is to maintain a global array which 
// stores the word count for each letter (how many words start 
// with ‘a’, how many words start with ‘b’, etc.). This array 
// will be updated by its threads, where each thread receives 
// some paths to text files from a client process. The threads 
// will modify the array to reflect the number of words starting 
// with a letter in those files. When all the client processes 
// have finished sending up requests, the server threads send 
// the final counts of the global array to the clients.

//  ---------------- PRINT STATEMENTS -----------------  //
// 1) Server Start                                      -XX
// 2) Each file path received (PID and path)             //
// 3) Each Acknowledge to client "ACK" (PID and path)    //
// 4) Each "END" received from Client (PID and path)     //
// 5) After receiving “END” from client (Thread id)      //
// 6) Server Ends                                        //

// (Process ID = (0,1,...n-1) n=total number of threads) //

int main(int argc, char **argv)
{
    LOG("--Server Started--");
    LOG("--Server Ended--");
}