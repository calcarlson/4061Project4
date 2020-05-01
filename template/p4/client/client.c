// Test Machine : csel-Vole-37
// Date : 04/10/20
// Name : Conrad Carlson , Nam Trinh, Jessica Cunningham
// x500 : Carl5362 , trinh064, Cunni536

#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h> 
#include <errno.h>
#include <time.h>
#define MSG_SIZE 512
// Display time stamp
void printtime(){
	time_t t = time(NULL);
  struct tm now = *localtime(&t);
  printf("[%d-%02d-%02d %02d:%02d:%02d]", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
	return;}
// Remove the last character of a string
void cuttail(char*a){
	int i=0;
	while(a[i]!='\n'){i++;}
	a[i]='\0';
	return;
	}
//find the bit length of a number
int len(int a){
	int i=0;
	while(true){a=a>>1;i++;if (a==0){return i;}}
	}
//determine the process number , process 0, 1,2, base on pid
int processnum(pid_t *pidlist, int size){
	int num=0;
	int bit=0;
	for (int i=0;i<size;i++)
	{if(pidlist[i]==0){bit=1;} else {bit=0;}
	 num=(num<<1)+bit;
		}
	return num;
	}

struct msg_buffer { 
    long msg_type; 
    char msg_text[MSG_SIZE]; 
} ; 

char * list[100];
// check if a file is a txt file
bool istext(char* path){
	int i=2;
	int f=0;
	int I=0;
	bool start=false;
	char* tail="txt";
	while(path[i]!='\0'){
		if(start){if (path[i]!=tail[i-I]) {break;} else {f++;} }
		if (path[i]=='.'){start=true;I=i+1;}
		i++;
		}
	
	return f==3;
	}
// traverse the folder for text files
int traverse(char *path) {	 
	static int index=0;
	 if (path==NULL){path=".";}
	 struct dirent **namelist;
           int n;
         
           n = scandir(path, &namelist, NULL, alphasort);
          
           if (n == -1) {
               return -1;
           }
            if (n == 2) {
               return -1;
           }    
          
          for (int i=2;i<n;i++) {
			  char  childpath[300];
			
			  sprintf(childpath,"%s/%s",path,namelist[i]->d_name);
			
			 if (istext(childpath))
			 {list[index]=(char*) calloc(100,sizeof(char*));
			 strcpy(list[index],childpath);
			 index++;}
			  free(namelist[i]);
			  traverse(childpath);
			  }
	return index;
}

int main(int argc, char** argv) {
	if (argc<3){printf("Not enough arg\n"); exit(1);}
	int c=atoi(argv[2]);
	key_t key=ftok("M",9);
	int msgid= msgget(key, IPC_CREAT | 0666);
//Performing folder traversing
	printf("Traversal starting ...\n");
	int txtcount=traverse(argv[1]);
	FILE *fp[c];
	
	
//Putting down the result
	for (int i=0; i<c;i++){	
		char path[100];
		sprintf(path,"./ClientInput/Client%d.txt",(i%c));
		fp[i]=fopen(path,"w+");
		}
		for (int i=0; i<txtcount;i++){
			fputs(list[i],fp[i%c]);
			fputs("\n",fp[i%c]);
			}
		
	for (int i=0; i<c;i++){	fclose(fp[i]);}
	int numbits=len(c);
//creating processes base on the number of times fork need to be called 
	pid_t pid[numbits];
	for (int i=0; i<numbits;i++)
	{pid[i]=fork();}
//end the extra unecessary processes 
	int pnum=processnum(pid,numbits);
	if(pnum>=c) {exit(1);}
	else{
		struct msg_buffer msgin,msgout;
// Set in message type to even, out message type to odd
		msgin.msg_type=pnum*2+2;
		msgout.msg_type=pnum*2+1;
		int curid=getpid();
		printf("Process %d starting...\n",pnum);
	char line[100];
  	FILE *fpt;
	char path[100];
	sprintf(path,"./ClientInput/Client%d.txt",(pnum));
	fpt=fopen(path,"r");
	while(fgets(line,100,fpt)!=NULL)
	{	cuttail(line);
		
		strcpy(msgout.msg_text,line);
	
	if(msgsnd(msgid,(void*)&msgout,sizeof(struct msg_buffer),0)==-1){
		printf("Send error %d",errno);exit(0);
		}
		printtime();
	printf("Process %d %d sending %s\n",pnum,curid,msgout.msg_text);
	
	if(msgrcv(msgid,(void*) &msgin,sizeof(struct msg_buffer),pnum*2+2,0)==-1)
	{printf("Receive error %d",errno);exit(0);}
	if(strcmp(msgin.msg_text,"ACK")!=0){printf("No ACK %d\n",pnum);exit(0);} 
	printtime();
	printf("Process %d %d received %s on %s\n",pnum,curid,msgin.msg_text,line);
	}
	fclose(fpt);
	strcpy(msgout.msg_text,"END");
	if(msgsnd(msgid,(void*)&msgout,sizeof(struct msg_buffer),0)==-1)
		{printf("Send error %d",errno);exit(0);}
	printtime();
	printf("Process %d %d sending %s\n",pnum,curid,msgout.msg_text);
	
	
	if(msgrcv(msgid,(void*) &msgin,sizeof(struct msg_buffer),pnum*2+2,0)==-1)
	{printf("Receive error %d",errno);exit(0);}
	printtime();
	printf("Process %d %d final receive:%s\n",pnum,curid,msgin.msg_text);
	char tosave[100];
		sprintf(tosave,"./Output/Client%d_out.txt",pnum);
		fpt=fopen(tosave,"w+");
		fputs(msgin.msg_text,fpt);
		fclose(fpt);
	if(pnum!=0){printf("Process%d terminating\n",pnum);exit(0);} else {for(int i=0;i<c;i++){wait(NULL);} }} 
	msgctl(msgid,IPC_RMID,NULL);
	printf("Process%d terminating\n",pnum);	
return 0;
}
