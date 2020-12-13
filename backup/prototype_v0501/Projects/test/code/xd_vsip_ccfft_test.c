#include <vsip.h>
#include <fftw3.h>
#include <sys/time.h>
#include <stdio.h>
#define length 10
#define count 1
void xd_ccfft_test(double **in,
                    fftw_complex *f_input,
                    fftw_complex *f_output,
                    fftw_plan plan){
    f_input = *in;
    int i = 0;
    if (plan == NULL){
        printf("NULL");
    }
    plan = fftw_plan_dft_1d(length,f_input,f_output,FFTW_FORWARD,FFTW_ESTIMATE);
    for (i = 0; i < length; i++)
    {
        // printf("%f,%f\n",f_input[i][0],f_input[i][1]);
        printf("in the function: %f,%f\n",f_output[i][0],f_output[i][1]);
    }
    
    fftw_execute(plan);
    for (i = 0; i < length; i++)
    {
        // printf("%f,%f\n",f_input[i][0],f_input[i][1]);
        printf("in the function: %f,%f\n",f_output[i][0],f_output[i][1]);
    }
    *in = f_output;
    
}
void test(){
    double *in = (double*)malloc(sizeof(double) * length * 2);
    int i = 0;
    for(i = 0; i < length * 2; i++){
        *(in+i) = i;
    }
    fftw_complex *d_input = (fftw_complex*)malloc(sizeof(fftw_complex) * length);
    fftw_complex *d_output = (fftw_complex*)malloc(sizeof(fftw_complex) * length);
    d_input = in;
    fftw_plan plan = fftw_plan_dft_1d(length,d_input,d_output,FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(plan);
    for (i = 0; i < length; i++)
    {
        printf("%d,real: %f, imag: %f\n",i,d_output[i][0],d_output[i][1]);
    }
    
}

int main(){
    double *input = (double *)malloc(sizeof(double)*length*2);

    double *output = (double *)malloc(sizeof(double)*length*2);
    int i = 0;
    for(i = 0; i < length * 2; i++){
        *(input+i) = i;
    }
    // test();
    fftw_complex *f_input = (fftw_complex*)malloc(sizeof(fftw_complex) * length);
    fftw_complex *f_output = (fftw_complex*)malloc(sizeof(fftw_complex) * length);
    fftw_plan plan = fftw_plan_dft_1d(length,f_input,f_output,FFTW_FORWARD,FFTW_ESTIMATE);
    
    xd_vsip_cblock_d *block_data = xd_vsip_cblockbind_d(input,NULL,length,XD_VSIP_MEM_CONST);
    xd_vsip_cvview_d *view_data = xd_vsip_vbind_d(block_data,0,1,length);
    xd_vsip_fft_d *fft_plan = xd_vsip_ccfftip_create_d(length,1,XD_VSIP_FFT_FWD,1,XD_VSIP_ALG_SPACE);
    
    struct timeval start,end;
    gettimeofday(&start,NULL);
    for (i = 0; i < count; i++)
    {
        xd_ccfft_test(&input,f_input,f_output,plan);
        // xd_vsip_ccfftip_d(fft_plan,view_data);
    }
    
    gettimeofday(&end,NULL);
    printf("speed time is:%f(ms)\n", ((double)(end.tv_sec * 1e6 + end.tv_usec) / 1000.0 - (double)(start.tv_sec * 1e6 + start.tv_usec) / 1000.0) / count); //time

    
    for (i = 0; i < length; i++)
    {
        printf("fftw format: the read part is: %f, the img part is: %f\n",input[i*2],input[i * 2 + 1]);
    }
    
}