#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>


#define ITERATIONS 2000000000
#define NTHREADS 4

double result[NTHREADS];

void *tfunc( void *args){
  
  int nthread=*((int *) args);
  int iStart = (nthread) * (ITERATIONS/NTHREADS);
  int iEnd = (nthread+1) * (ITERATIONS/NTHREADS);
	
  for (; iStart < iEnd; iStart++) {
      result[nthread] += pow(-1.0,iStart) / ((2*iStart) + 1);
  }
  
}

int main(int argc, char const *argv[]) {

        struct timeval start;
        struct timeval end;
        int parArr[NTHREADS];
        double fResult = 0;
        pthread_t tid[NTHREADS];

        printf("PI/4 value in math.h: %1.48lf\n",M_PI/4 );
        gettimeofday(&start, NULL);


        for(int i = 0; i < NTHREADS; i++){
          parArr[i]=i;
          pthread_create(&tid[i],NULL,tfunc,(void *) &parArr[i]);
        }

        for(int i = 0; i < NTHREADS; i++)
          pthread_join(tid[i],NULL);

        gettimeofday(&end, NULL);


        for (int i = 0; i < NTHREADS; i++) {
          fResult += result[i];
        }

        printf("Leibniz result: %1.48lf\n", fResult );
        printf("Total execution time: %li\n", end.tv_sec - start.tv_sec);



        return 0;
}
