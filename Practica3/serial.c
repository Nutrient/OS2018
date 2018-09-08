#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>


int main(int argc, char const *argv[]) {

        struct timeval start;
        struct timeval end;

        double result = 0;

        printf("PI/4 value in math.h: %1.48lf\n",M_PI/4 );

        gettimeofday(&start, NULL);

        for (int i = 0; i < 2000000000; i++) {
            result += pow(-1.0,i) / ((2*i) + 1);
        }


        gettimeofday(&end, NULL);
        printf("Leibniz result: %1.48lf\n", result/4 );
        printf("Total execution time: %li.%li\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec );

        return 0;
}
