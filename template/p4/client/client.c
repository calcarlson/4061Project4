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

struct msg_buffer { 
    long msg_type; 
    char msg_text[MSG_SIZE]; 
} msg; 

char * list[50];

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
			 {list[index]=(char*) calloc(50,sizeof(char*));
			 strcpy(list[index],childpath);
			 index++;}
			  free(namelist[i]);
			  traverse(childpath);
			  }
	return index;
}

int main(int argc, char** argv) {
	
	int c=atoi(argv[2]);
	int txtcount=traverse(argv[1]);
    char toserver[256];
		for (int i=0; i<txtcount;i++){
			FILE *fp;
			char path[50];
			sprintf(path,"./ClientInput/Client%d.txt",(i%c));
			fp=fopen(path,"a");
			fputs(list[i],fp);
			fputs("\n",fp);
			fclose(fp);
			}
		//printf("%s",toserver);
	

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
    return 0; 
}
