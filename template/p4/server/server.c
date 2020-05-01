// Test Machine : csel-Vole-37
// Date : 04/10/20
// Name : Conrad Carlson , Nam Trinh, Jessica Cunningham
// x500 : Carl5362 , Namht1999, Cunni536

#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include <semaphore.h>
#include <time.h>
#define MSG_SIZE 512
pthread_mutex_t lock;
void printtime(){
	time_t t = time(NULL);
  struct tm now = *localtime(&t);
  printf("[%d-%02d-%02d %02d:%02d:%02d]", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	return;}

struct msg_buffer { 
    long msg_type; 
    char msg_text[MSG_SIZE]; 
} ; 

struct superbuffer{
	
	int msgid;
	int c;
	int* arr;
	int check;
	};

	
void analz (char* path, int* array){
	FILE* fp;
	fp=fopen(path,"r");
	char line[100];
	while(fgets(line,100,fp)!=NULL)
	{	int first=(int)line[0];
	    if (first<91) {first-=65;}
	    else {first-=97;}
	    array[first]++;
		}
	fclose(fp);
	return;
	}

void function(void* arg) {
	
struct superbuffer* package=(struct superbuffer*)arg;
pthread_mutex_lock(&lock);
int pnum=--package->c;
pthread_mutex_unlock(&lock);
printf("Thread %d starting ...\n",pnum);
int msgid=package->msgid;
struct msg_buffer msgin,msgout;
msgin.msg_type=pnum*2+1;
msgout.msg_type=pnum*2+2; 
char path[300];
int * arr=package->arr;
char final[200];

	while(1)
    {	
		
		if(msgrcv(msgid,(void*) &msgin,sizeof(struct msg_buffer),pnum*2+1,0)==-1)
		{printf("Receive error %d",errno);}
   
   strcpy(path,msgin.msg_text);
     printtime();printf("Thread %d %ld received %s\n",pnum,pthread_self(),path);
     
     if(strcmp(msgin.msg_text,"END")==0){break;}
     //sem_wait(&sem);
     //package->msg[pnum].msg_type=pnum*2+2;
    printtime();
    printf("Thread %d %ld sending ACK on %s\n",pnum,pthread_self(),path);
    strcpy(msgout.msg_text,"ACK");
    if(msgsnd(msgid,(void*)&(msgout),sizeof(struct msg_buffer),0)==-1){printf("Send error %d\n",errno);exit(0);}
    pthread_mutex_lock(&lock);
    analz(path,arr);
    pthread_mutex_unlock(&lock);		
	}
    pthread_mutex_lock(&lock);
   --package->check;
   pthread_mutex_unlock(&lock);
   while(package->check!=0);
    for(int i=0;i<26;i++){ 
		char tocat[10];
		if(i!=25){sprintf(tocat,"%d#",arr[i]);} 
		else{sprintf(tocat,"%d",arr[i]);}
		strcat(final,tocat);}
	strcpy(msgout.msg_text,final);
    //pthread_mutex_lock(&lock);
    //package->msg[pnum].msg_type=pnum*2+2;
    msgsnd(msgid,(void*)&(msgout),sizeof(struct msg_buffer),0);
	printtime();printf("Thread %d %ld sending final result\n",pnum,pthread_self());
	//pthread_mutex_unlock(&lock);
    //printf("mark\n");
return;
}

int main(int argc, char** argv) {
//	printf("%d %d %d %d %d %d %d\n",EAGAIN,EACCES,EFAULT,EIDRM,EINTR,EINVAL,ENOMEM);
	if (argc<2){printf("Not enough arg\n"); exit(1);}
	//printf("%s\n",argv[1]);
	int c=atoi(argv[1]);
	//sem_init(&sem, 0, 0);
	pthread_mutex_init(&lock, NULL);
    key_t key=ftok("M",9); ; 
    int msgid=msgget(key, IPC_CREAT | 0666); 
   msgctl(msgid,IPC_RMID,NULL);
   msgid=msgget(key, IPC_CREAT | 0666);
  int arr[26];
  for(int i=0;i<26;i++){arr[i]=0;}
   struct superbuffer topass={msgid,c,arr,c} ;
    pthread_t thread[c];
   
    for (int i=0; i < c; i++) {
		pthread_create(&thread[i], NULL, function, (void*) &topass);}
    for (int i=0; i < c; i++) pthread_join(thread[i], NULL);
   // 
    return 0; 
}
