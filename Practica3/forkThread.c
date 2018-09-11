#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define ITERATIONS 2000000000
#define NTHREADS 4

int main(int argc, char const *argv[]){
	struct timeval start;
        struct timeval end;
	double fResult = 0;
	key_t key = ftok("./makefile",8);
	int shmid = shmget(key,sizeof(double) * NTHREADS,0666| IPC_CREAT);
	double *result = (double*) shmat(shmid,(void*)0,0);
	printf("PI/4 value in math.h: %1.48lf\n",M_PI/4 );
        gettimeofday(&start, NULL);

	for(int i=0; i<NTHREADS; i++){
		
		if(fork() == 0){
		    int iStart = (i) * (ITERATIONS/NTHREADS);
		    int iEnd = (i+1) * (ITERATIONS/NTHREADS);
		    
		    for (; iStart < iEnd; iStart++) {
		        result[i] += pow(-1.0,iStart) / ((2*iStart) + 1);
		    }
		    exit(0);
		}
	}
	for(int i=0; i<NTHREADS; i++)
		wait(NULL);
			
	for (int i = 0; i < NTHREADS; i++) {
          fResult += result[i];
        }

	gettimeofday(&end, NULL);
	printf("Leibniz result: %1.48lf\n", fResult );
        printf("Total execution time: %li\n", end.tv_sec - start.tv_sec);
	shmdt(result);
	shmctl(shmid,IPC_RMID,NULL);
}