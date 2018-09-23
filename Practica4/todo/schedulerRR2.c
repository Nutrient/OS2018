#include <scheduler.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready,callingthread);
	}
	
	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}
	if(event==TIMER){
		key_t key = ftok("./schedulerRR2",65);
		int shmid = shmget(key,sizeof(int),0666|IPC_CREAT);
		int *qCounter = (int *) shmat(shmid,(void*)0,0);
		
		if(*qCounter == 1){
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
			changethread=1;
			shmctl(shmid,IPC_RMID,NULL);
		}
		else{
			*qCounter = 1;
		}
		shmdt(qCounter);
		
		
	}

	
	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
