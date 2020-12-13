#include <vsip.h>
#include "constant.h"
#include <stdio.h>
// #include "fft.c"
/**
 * 计算频域脉压并按照脉冲号、距离门号重排数据
 * 
 * echo: 回波信号，600*16
 * echo_pulse: 回波信号一次脉冲的数据
 * echo_fft: 回波信号一次脉冲的傅立叶变换结果
 * coeff_fft: 脉冲压缩系数的傅里叶变换结果
 * pc_fft: 脉冲压缩
 * pc0: 逆傅里叶变换
 * pc1: 数据截取之后的
 * pc: 输出结果
 * echoPlan: echo的傅里叶计算
 * pcPlan:pc的傅里叶计算
 * 
*/
void frequencyDomainPulseCompression(vsip_cmview_f *echo, vsip_cmview_f *echo_pulse,
                                     vsip_cmview_f *echo_fft, vsip_cmview_f *coeff_fft,
                                     vsip_cmview_f *pc0, vsip_cmview_f *pc, vsip_cmview_f *pc_fft,
                                     vsip_fftm_f *echoPlan, vsip_fftm_f *pcPlan)
{
    int i,j,count = 0;
    echo_pulse = vsip_cmsubview_f(echo,0,0,1,SampleNumber);
    for (i = 0;i < PulseNumber;i++){
        vsip_ccfftmop_f(echoPlan, echo_pulse, echo_fft);
        vsip_cmmul_f(echo_fft, coeff_fft, pc_fft);
        vsip_ccfftmop_f(pcPlan, pc_fft, pc0);
        for (j = 0; j < SampleNumber; j++)
        {
            vsip_cmput_f(pc, i, j, vsip_cmget_f(pc0, 0, j));
        }
        vsip_cmputoffset_f(echo_pulse, (count += SampleNumber) % (SampleNumber * PulseNumber));
    }
}

