#include <scheduler.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[5];
QUEUE waitinginevent[MAXTHREAD];

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	key_t key = ftok("./schedulerRetro",65);
	int shmid = shmget(key,sizeof(int)*MAXTHREAD,0666|IPC_CREAT);
	int *queueLocation = (int *) shmat(shmid,(void*)0,0);

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready[0],callingthread);
	
		queueLocation[callingthread] = 0;
		shmdt(queueLocation);
		
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
			_enqueue(&ready[queueLocation[callingthread]],callingthread);
			
			shmdt(queueLocation);
			
	}
	
	if(event==TIMER){		
		threads[callingthread].status=READY;
		
		if(queueLocation[callingthread] >= 5)
			_enqueue(&ready[queueLocation[callingthread]],callingthread);
		else
			_enqueue(&ready[++queueLocation[callingthread]],callingthread);
		
		changethread=1;
	}
	
	if(changethread)
	{
		old=currthread;
		if(!_emptyq(&ready[0]))
			next=_dequeue(&ready[0]);
		else if(!_emptyq(&ready[1]))
			next=_dequeue(&ready[1]);
		else if(!_emptyq(&ready[2]))
			next=_dequeue(&ready[2]);
		else if(!_emptyq(&ready[3]))
			next=_dequeue(&ready[3]);
		else if(!_emptyq(&ready[4]))
			next=_dequeue(&ready[4]);
		else if(!_emptyq(&ready[5]))
			next=_dequeue(&ready[5]);

		shmdt(queueLocation);
		if(_emptyq(&ready[0]) && _emptyq(&ready[1]) && _emptyq(&ready[2]) && _emptyq(&ready[3]) && _emptyq(&ready[4]) && _emptyq(&ready[5]))
			shmctl(shmid,IPC_RMID,NULL);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
