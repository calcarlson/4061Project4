#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
 #include <dirent.h>
 #include <sys/wait.h> 
 
#define MSG_SIZE 512

int len(int a){
	int i=0;
	while(true){a=a>>1;i++;if (a==0){return i;}}
	}
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
} msg; 

char * list[100];

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
	//printf("%d",f);
	return f==3;
	}

int traverse(char *path) {	 
	static int index=0;
	 if (path==NULL){path=".";}
	 struct dirent **namelist;
           int n;
         
           n = scandir(path, &namelist, NULL, alphasort);
          // printf("                number of files inside    %d \n",n);
           if (n == -1) {
               return -1;
           }
            if (n == 2) {
               return -1;
           }    
          
          for (int i=2;i<n;i++) {
			  char  childpath[300];
			 // printf(" in for loop %s\n",namelist[i]->d_name);
			  sprintf(childpath,"%s/%s",path,namelist[i]->d_name);
			 // printf("%s\n",childpath);
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
	
	int txtcount=traverse(argv[1]);
	FILE *fp[c];
	for (int i=0; i<c;i++){	
		char path[100];
		sprintf(path,"./ClientInput/Client%d.txt",(i%c));
		fp[i]=fopen(path,"w+");
		}
	
		for (int i=0; i<txtcount;i++){
			char path[100];
			fputs(list[i],fp[i%c]);
			fputs("\n",fp[i%c]);
			}
		//printf("%s",toserver);
	for (int i=0; i<c;i++){	fclose(fp[i]);}
	int numbits=len(c);
	//printf("numbit%d",c);
	
	//pid_t first;
	//first=fork();
	pid_t pid[numbits];
	for (int i=0; i<numbits;i++)
	{pid[i]=fork();}
	//pid[0]=fork();
	//pid[1]=fork();
	//pid[2]=fork();
	int pnum=processnum(pid,numbits);
	if(pnum>=c) {exit(1);}
	
	else{
		int curid=getpid();
		printf("Client %d starting...\n",pnum);
	char line[100];
	//sprintf(line,"%d\n",pnum);
	//execl("/bin/echo","echo",line,(char*)NULL);
	
	FILE *fpt;
	char path[100];
	sprintf(path,"./ClientInput/Client%d.txt",(pnum));
	fpt=fopen(path,"r");
	while(fgets(line,100,fpt)!=NULL)
	{
		msg.msg_type=pnum*2+1;
		strcpy(msg.msg_text,line);
	msgsnd(msgid,(void*)&msg,sizeof(msg),0);
	msgrcv(msgid,(void*) &msg,sizeof(msg),pnum*2+2,0);
	printf("On Client%d: Process %d received %s\n",pnum,curid,msg.msg_text);
	}
	msg.msg_type=pnum*2+1;
	strcpy(msg.msg_text,"END");
	msgsnd(msgid,(void*)&msg,sizeof(msg),0);
	msgrcv(msgid,(void*) &msg,sizeof(msg),pnum*2+2,0);
	printf("On Client%d: Process %d final receive:%s\n",pnum,curid,msg.msg_text);
	fclose(fpt);
	if(pnum==c-1){exit(0);} else {wait(NULL);if (pnum!=0) {exit(0);} } 
	
	}
	msgctl(msgid,IPC_RMID,NULL);
	//exit(1);
	
		
/*
	msg.msg_type=3;
	strcpy(msg.msg_text,toserver);
    key_t key=ftok("M",10); 
    int msgid= msgget(key, IPC_CREAT | 0666);
	msgsnd(msgid,(void*)&msg,sizeof(msg),0);
	pid_t pid=fork();
	if (pid==0){system("./server");exit(1);} // Put executable server file name here
	else 
		{wait(NULL);}
		
	msgrcv(msgid,(void*) &msg,sizeof(msg),3,0);
	printf("On client %s\n",msg.msg_text);
	msgctl(msgid,IPC_RMID,NULL);
     
    */
return 0;
}
