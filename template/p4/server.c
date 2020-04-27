#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <semaphore.h>

#define MSG_SIZE 512
pthread_mutex_t lock; 
sem_t sem;
struct msg_buffer { 
    long msg_type; 
    char msg_text[MSG_SIZE]; 
} ; 

struct superbuffer{
	struct msg_buffer* msg;
	int msgid;
	int c;
	};
void function(void* arg) {
	
struct superbuffer* package=(struct superbuffer*)arg;
pthread_mutex_lock(&lock);
int pnum=--package->c;
pthread_mutex_unlock(&lock);
printf("Thread %d\n",pnum);
int msgid=package->msgid;
	while(1)
    {
		package->msg[pnum].msg_type=pnum*2+1;
		msgrcv(msgid,(void*) &(package->msg[pnum]),sizeof(struct msg_buffer),pnum*2+1,0);
		//pthread_mutex_lock(&lock); 	
   //sem_post(&sem);
     printf("On server: Thread %ld %s\n",pthread_self(),package->msg[pnum].msg_text);
     
     if(strcmp(package->msg[pnum].msg_text,"END")==0){pthread_mutex_unlock(&lock);break;}
     //sem_wait(&sem);
    strcpy(package->msg[pnum].msg_text,"ACK");
    package->msg[pnum].msg_type=pnum*2+2;
    msgsnd(msgid,(void*)&(package->msg[pnum]),sizeof(struct msg_buffer),0);
    //pthread_mutex_unlock(&lock);		
	}
    strcpy(package->msg[pnum].msg_text,"DONE");
    //pthread_mutex_lock(&lock);
    package->msg[pnum].msg_type=pnum*2+2;
    msgsnd(msgid,(void*)&(package->msg[pnum]),sizeof(struct msg_buffer),0);
    
	//pthread_mutex_unlock(&lock);
    printf("mark\n");
return;
}

int main(int argc, char** argv) {
	if (argc<2){printf("Not enough arg\n"); exit(1);}
	//printf("%s\n",argv[1]);
	int c=atoi(argv[1]);
	//sem_init(&sem, 0, 0);
	pthread_mutex_init(&lock, NULL);
    key_t key=ftok("M",9); ; 
    int msgid=msgget(key, IPC_CREAT | 0666); 
   msgctl(msgid,IPC_RMID,NULL);
   msgid=msgget(key, IPC_CREAT | 0666);
   struct msg_buffer msg[c];
   struct superbuffer topass={msg,msgid,c} ;
    pthread_t thread[c];
   
    for (int i=0; i < c; i++) {
		pthread_create(&thread[i], NULL, function, (void*) &topass);}
    for (int i=0; i < c; i++) pthread_join(thread[i], NULL);
   // 
    return 0; 
}
