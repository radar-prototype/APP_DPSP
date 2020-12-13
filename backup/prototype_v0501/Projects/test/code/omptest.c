#include <omp.h>
#include <time.h>
#include <stdio.h>

int main(){
    int a = 0;
    int i = 0;
    int j = 0;
    double avgTime = 0.0;
    for (j = 0;j < 100;j++){
        double start,finish;
        start = omp_get_wtime();
        //#pragma omp parallel for
        for (i = 0;i < 1000000;i++){
            a++;
        }
        finish = omp_get_wtime();
        avgTime += (finish - start);
    }
    printf("%f s\n",avgTime/100);
    return 0;
}