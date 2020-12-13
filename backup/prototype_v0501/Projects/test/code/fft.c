#include <complex.h>
#include "fftw3.h"
#include "vsip.h"

/**
 * in 是输入
 * out 是输出
 * in_fftw 是做运算的输入
 * out_fftw 是做运算的输出
 * plan 是fft运算的plan
 * n 是做fft运算的长度
 * row_or_col 1: 是行向量 0: 是列向量
 * fwd_or_bwd 1: 是前向fft 0: 是后向fft
*/
void fft_1d(vsip_cmview_f *in,
            vsip_cmview_f *out,
            fftw_complex *in_fftw3,
            fftw_complex *out_fftw3,
            fftw_plan plan, int n,
            int fwd_or_bwd, int row_or_col)
{
    int i, j;
    if (row_or_col == 1)
    {
        for (i = 0; i < vsip_cmgetrowlength_f(in); i++)
        {
            in_fftw3[i] = vsip_cmget_f(in, 0, i).r+vsip_cmget_f(in, 0, i).i*I;
        }
    }
    else if (row_or_col == 0)
    {
        for (i = 0; i < vsip_cmgetcollength_f(in); i++)
        {
            in_fftw3[i] = vsip_cmget_f(in, 1, 0).r + vsip_cmget_f(in, 1, 0).i*I;
        }
    }
    if (fwd_or_bwd == 1)
    {
        plan = fftw_plan_dft_1d(n, in_fftw3, out_fftw3, FFTW_FORWARD, FFTW_ESTIMATE);
    }
    else if (fwd_or_bwd == 0)
    {
        plan = fftw_plan_dft_1d(n, in_fftw3, out_fftw3, FFTW_BACKWARD, FFTW_ESTIMATE);
    }

    fftw_execute(plan);
    if (row_or_col == 1)
    {
        for (i = 0; i < vsip_cmgetrowlength_f(out); i++)
        {
            vsip_cmput_f(out, 0, i, vsip_cmplx_f(creal(out_fftw3[i]),cimag(out_fftw3[i])));
        }
    }
    else if (row_or_col == 0)
    {
        for (i = 0; i < vsip_cmgetrowlength_f(out); i++)
        {
            vsip_cmput_f(out, i, 0, vsip_cmplx_f(creal(out_fftw3[i]),cimag(out_fftw3[i])));
        }
    }
}