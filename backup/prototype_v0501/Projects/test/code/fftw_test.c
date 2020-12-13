#include <fftw3.h>

int main(){
    double *in = (double*)malloc(sizeof(double) * 10);
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        in[i] = i;
    }
    fftw_plan plan = fftw_plan_dft_1d(5,in,in,FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(plan);
    for (i = 0; i < 5; i++)
    {
        printf("%f,%fi\n",in[i*2],in[i*2+1]);
    }
    
    
}