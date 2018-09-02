#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

int main(int argc, char *argv[]){
	pid_t p;
	int salir=0;
	char command[80];
	char *argAux;
	char cwd[80];
	int i;

	while(command != "exit"){
		char *args[80];
		i=0;
		getcwd(cwd,80);
		printf("\nsh %s>$ ",cwd);
		fgets(command,80,stdin);
		fflush(stdin);
		argAux = strtok(command,"\n");
		argAux = strtok(argAux," ");
		while(argAux != NULL){
			//printf("current argAux val: %s\n",argAux);
			args[i++] = argAux;
			
			
			/*if(i>0)
				printf("last args val: %s\n",args[i-1]);
			printf("current args val: %s\n",args[i++]);*/
			
			argAux = strtok(NULL," ");
						
			
		}
		if(i==0){}
		else if(strcmp(args[0],"exit") == 0){
			printf("test\n");
			kill(getpid(),SIGKILL);
		}
		else if(strcmp(args[0],"shutdown") == 0){
			char *temp[80]={"pkill","xterm",NULL};
			execvp(temp[0],temp);
		}
		else if(strcmp(args[0],"cd") == 0){
			if(i==1)
				chdir("/");			
			else
				chdir(args[1]);
		}
		
		else {
			p=fork();
			if(p==0){
				//printf("executing process\n");
				args[i] = NULL;
				if(strncmp(args[0],"./",2) == 0)
					execvp(args[0],args);
				else if(strncmp(args[0],"/",1) == 0){
					
					int dirAmmount=0;
					char *temp = strtok(cwd,"/");
					while(temp != NULL){
						dirAmmount++;
						temp = strtok(NULL,"/");
					}
					char newLocation[dirAmmount*3 + strlen(args[0])];
					strcpy(newLocation,"");
					for(;dirAmmount>0;dirAmmount--)
						strcat(newLocation,"/..");
					strcat(newLocation,args[0]);
					execvp(newLocation,args);
				}
				
				else{
					execvp(args[0],args);	
				}
				
			}
			else{
				
				if(strcmp(args[i - 1], "&") == 0){
					//printf("background process\n");
				}
				else{
					//printf("waiting for process to finish\n");
					wait(NULL);
				}
					

			}
		}

		
	}
}