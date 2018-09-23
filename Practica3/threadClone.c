#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>

#define ITERATIONS 2000000000
#define NTHREADS 4
#define FIBER_STACK 1024*64

double result[NTHREADS];

int tfunc( void *args){
        int nthread=*((int *) args);
        int iStart = (nthread) * (ITERATIONS/NTHREADS);
        int iEnd = (nthread+1) * (ITERATIONS/NTHREADS);
        //printf("NTHREAD = %i | result Address = %p\n", nthread, &result[nthread]);
        for (; iStart < iEnd; iStart++) {
                result[nthread] += pow(-1.0,iStart) / ((2*iStart) + 1);
        }
        return 0;
}

int main(int argc, char const *argv[]) {

        struct timeval start;
        struct timeval end;
        int parArr[NTHREADS];
        int status = 0;
        pid_t pidStack[NTHREADS];
        double fResult = 0;
        pthread_t tid[NTHREADS];
        void *stack = malloc(FIBER_STACK * NTHREADS);

        printf("PI/4 value in math.h: %1.48lf\n",M_PI/4 );
        gettimeofday(&start, NULL);


        for(int i = 1; i <= NTHREADS; i++) {
                parArr[i]=i-1;
                pidStack[i] = clone(&tfunc, (char *) stack + (FIBER_STACK*i), SIGCHLD | CLONE_VM, (void *) &parArr[i] );
                //pid_t pid = waitpid(pidStack[i],&status, 0);
        }


        for(int i = 0; i < NTHREADS; i++) {
                //pidStack[i] = waitpid(pidStack[i],&status, 0);
                pidStack[i] = wait(&status);
        }

        gettimeofday(&end, NULL);
        free(stack);
        for (int i = 0; i < NTHREADS; i++) {
                printf("result[%i] = %1.48lf\n",i, result[i]);
                fResult += result[i];
        }

        printf("Leibniz result: %1.48lf\n", fResult );
        printf("Total execution time: %li\n", end.tv_sec - start.tv_sec);
        return 0;
}
