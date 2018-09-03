#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(){
	pid_t p;
	int counter=0;
	int status=0;
	
	while(counter < 6){
		p=fork();
		if(p==0)
			execlp("xterm","myuniqueprocess","-e","./getty",NULL);
		else{
			if(++counter==6){
				wait(&status);
				if(status == 3840)
					exit(0);
				counter--;
			}
			
		}	
		
	}
}