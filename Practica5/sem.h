#define MAXPROCESS 50
#define CYCLES 5

typedef struct SEM{
	int id;
	short val;
	short current;
	short pop;
	int queue[MAXPROCESS];
}SEM;


void waitsem(SEM *sem);
void sigsem(SEM *sem);
void initsem(SEM *sem, short val);