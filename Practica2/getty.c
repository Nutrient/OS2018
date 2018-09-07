#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>


int main(){

	pid_t p;
	char *user_x;
	char *pass_x;
	char readLine[100];
	int access=0;
	int status =0;
	FILE *file;
	

	while(1==1){
		char user[100];
		char pass[100];
		printf("Enter Username: ");
		fgets(user,80,stdin);
		printf("Enter Password: ");
		fgets(pass,80,stdin);

		file = fopen("./passwd","r");
		while(fgets(readLine,100,file) != NULL){
//			printf("%s",readLine);
			strtok(user,"\n");
			user_x = strtok(readLine,":");
			pass_x = strtok(NULL,":");
			if(strcmp(user,user_x) == 0 && strcmp(pass,pass_x) == 0){
				access = 1;
				break;
			}
		}
		if(access == 1){
			p=fork();
			if(p==0){
				char *args[]={"./sh",NULL};
				execvp(args[0],args);
				
			}
			wait(&status);
			access =0;
		}
		
	}
	
	
	
		
	

}
