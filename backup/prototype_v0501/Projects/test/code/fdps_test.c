#include <vsip.h>
#include "readFile.h"
#include "writeFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
void frequencyDomainPulseCompression(xd_vsip_cmview_f *echo, xd_vsip_cmview_f *echo_pulse,
                                     xd_vsip_cmview_f *echo_fft, xd_vsip_cmview_f *coeff_fft,
                                     xd_vsip_cmview_f *pc0, xd_vsip_cmview_f *pc, xd_vsip_cmview_f *pc_fft,
                                     xd_vsip_fftm_f *echoPlan, xd_vsip_fftm_f *pcPlan)
{
    int i,j,count = 0;
    echo_pulse = xd_vsip_cmsubview_f(echo,0,0,1,SampleNumber);
    for (i = 0;i < PulseNumber;i++){
        xd_vsip_ccfftmop_f(echoPlan, echo_pulse, echo_fft);
        xd_vsip_cmmul_f(echo_fft, coeff_fft, pc_fft);
        xd_vsip_ccfftmop_f(pcPlan, pc_fft, pc0);
        for (j = 0; j < SampleNumber; j++)
        {
            xd_vsip_cmput_f(pc, i, j, xd_vsip_cmget_f(pc0, 0, j));
        }
        xd_vsip_cmputoffset_f(echo_pulse, (count += SampleNumber) % (SampleNumber * PulseNumber));
    }
}
void frequencyDomainPulseCompression1(xd_vsip_cmview_f *echo, xd_vsip_cmview_f *echo_pulse,
                                     xd_vsip_cmview_f *echo_fft, xd_vsip_cmview_f *coeff_fft,
                                     xd_vsip_cmview_f *pc0, xd_vsip_cmview_f *pc, xd_vsip_cmview_f *pc_fft,
                                     xd_vsip_fftm_f *echoPlan, xd_vsip_fftm_f *pcPlan)
{
    int i,j,count = 0;
    echo_pulse = xd_vsip_cmsubview_f(echo,0,0,1,SampleNumber);
    for (i = 0;i < PulseNumber;i++){
        xd_vsip_ccfftmop_f(echoPlan, echo_pulse, echo_fft);
        xd_vsip_cmmul_f(echo_fft, coeff_fft, pc_fft);
        xd_vsip_ccfftmop_f(pcPlan, pc_fft, pc0);
        for (j = 0; j < SampleNumber; j++)
        {
            xd_vsip_cmput_f(pc, i, j, xd_vsip_cmget_f(pc0, 0, j));
        }
        xd_vsip_cmputoffset_f(echo_pulse, (count += SampleNumber) % (SampleNumber * PulseNumber));
    }
}
int main(){
    float* coeff = (float*)malloc(sizeof(float) * SampleNumber * 2);
    float *output = (float*)malloc(sizeof(float) * SampleNumber * 2);
    float *fdpc_input = (float*)malloc(sizeof(float) * SampleNumber * 2);
    int i = 0;
    for (i = 0; i < 2*SampleNumber; i++)
    {
        coeff[i] = 0;
        output[i] = 0;
        fdpc_input[i] = i;
    }
    read_data_as_float("./coeff.txt",coeff,0,0);
    xd_vsip_cblock_f *c_cbock = xd_vsip_cblockbind_f(coeff,NULL,SampleNumber,XD_VSIP_MEM_CONST);
    xd_vsip_cvview_f *c_view = xd_vsip_cvbind_f(c_cbock,0,1,SampleNumber);
    fftwf_plan coeff_plan = fftwf_plan_dft_1d(SampleNumber,coeff,coeff,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_execute(coeff_plan);
    // read_data_as_float("./test.txt",fdpc_input,0,0);

    xd_vsip_cblock_f *f_block = xd_vsip_cblockbind_f(fdpc_input,NULL,SampleNumber,XD_VSIP_MEM_CONST);
    xd_vsip_cvview_f *f_view = xd_vsip_cvbind_f(f_block,0,1,SampleNumber);

    xd_vsip_cblock_f *o_block = xd_vsip_cblockbind_f(output,NULL,SampleNumber,XD_VSIP_MEM_CONST);
    xd_vsip_cvview_f *o_view = xd_vsip_cvbind_f(o_block,0,1,SampleNumber);
    fftwf_plan fdpc_plan = fftwf_plan_dft_1d(SampleNumber,fdpc_input,fdpc_input,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_plan output_plan = fftwf_plan_dft_1d(SampleNumber,fdpc_input,fdpc_input,FFTW_BACKWARD,FFTW_ESTIMATE);
    struct timeval t1, t2, t3;

    xd_vsip_fft_d *plan1 = xd_vsip_ccfftop_create_f(SampleNumber,1,XD_VSIP_FFT_FWD,1,XD_VSIP_ALG_TIME);
    xd_vsip_fft_d *plan3 = xd_vsip_ccfftop_create_f(SampleNumber,1,XD_VSIP_FFT_INV,1,XD_VSIP_ALG_TIME);
    gettimeofday(&t1, NULL);
    
    xd_vsip_ccfftip_f(plan1,f_view);
    // fftwf_execute(fdpc_plan);
    xd_vsip_cvmul_f(c_view,f_view,o_view);
    // fftwf_execute(output_plan);
    xd_vsip_ccfftip_f(plan3,o_view);
    gettimeofday(&t2, NULL);
    printf("speed time is:%f(ms)\n", (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0); //time
    
}