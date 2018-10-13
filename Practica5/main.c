#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include "sem.h"

#define atomic_xchg(A,B) __asm__ __volatile__( \
" lock xchg %1,%0 ;\n" \
: "=ir" (A) \
: "m" (B), "ir" (A) \
);


char *pais[3]={"Peru","Bolivia","Colombia"};
int *g;
SEM *mySem;

/*void process(int i){
	int k,l;
	for(k=0;k<CYCLES;k++){
		l=1;
		do{ atomic_xchg(l,*g);} while(l!=0);
		printf("Entra %s", pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf(" - %s Sale\n",pais[i]);
		l=1;
		*g=0;
		sleep(rand()%3);
	}
	exit(0);
}*/

void process(int i){
	int k;
	for(k=0;k<CYCLES;k++){
		waitsem(mySem);
		
		printf("Entra %s", pais[i]);
		fflush(stdout);
		sleep(rand()%3);
		printf(" - %s Sale\n",pais[i]);
		
		sigsem(mySem);
		
		sleep(rand()%3);
	}
	exit(0);
}

void initsem(SEM *sem, short val){
	int l=1;
	do{ atomic_xchg(l,*g);} while(l!=0);
	sem->id = (int) getpid();
	sem->val = val;
	sem->current = 0;
	sem->pop = 0;
	*g = 0;
}

void sigsem(SEM *sem){
	int l=1;
	do{ atomic_xchg(l,*g);} while(l!=0);
	sem->val++;
	if(sem->current == sem->pop){
		*g=0;
		return;
	}
	int pid = sem->queue[sem->pop];
	if(sem->pop == MAXPROCESS)
		sem->pop=0;
	else
		sem->pop++;
	kill(pid,SIGCONT);
	*g=0;
}

void waitsem(SEM *sem){
	int l;
	
	while(1){
		l=1;
		do{ atomic_xchg(l,*g);} while(l!=0);
		if(sem->val > 0){
			sem->val--;
			*g=0;
			return;
		}
		else{
			sem->queue[sem->current] = (int) getpid();
			if(sem->current == MAXPROCESS)
				sem->current=0;
			else
				sem->current++;
			*g=0;
			kill(getpid(),SIGSTOP);
		}
	}
}

int main(){
	int pid,status, shmid, shmidsem, i;

	shmid=shmget(0x1234,sizeof(g),0666|IPC_CREAT);
	shmidsem=shmget(0x1235,sizeof(mySem),0666|IPC_CREAT);
	
	g=shmat(shmid,NULL,0);
	mySem=(SEM *)shmat(shmidsem,NULL,0);

	*g=0;
	initsem(mySem, 1);
	srand(getpid());
	
	for(i=0;i<3;i++){
		pid=fork();
		if(pid==0)
			process(i);
	}

	for(i=0;i<3;i++)
		pid = wait(&status);

	
	shmdt(g);
	shmdt(mySem);
	shmctl(shmid,IPC_RMID,NULL);
	shmctl(shmidsem,IPC_RMID,NULL);

	return 0;
}