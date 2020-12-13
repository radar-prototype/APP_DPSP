#include <stdio.h>
#include "vsip.h"
int main(){
    xd_vsip_init((void*)0);
    /*xd_vsip_index length = 10;
    xd_vsip_scalar_f scale = 1;
    xd_vsip_fft_dir dir = XD_VSIP_FFT_FWD;
    xd_vsip_alg_hint hint = XD_VSIP_ALG_SPACE;
    xd_vsip_length ntimes = 10;
    printf(xd_vsip_ccfftip_create_f(length,scale,dir,ntimes,hint));*/
    xd_vsip_mview_f *a = xd_vsip_mcreate_f(50,25,XD_VSIP_ROW,XD_VSIP_MEM_NONE);
    xd_vsip_vview_f *b = xd_vsip_vcreate_f(50,0);
    xd_vsip_vview_f *c = xd_vsip_vcreate_f(50,0);
    xd_vsip_vfill_f(5,b);
    xd_vsip_mfill_f(5,a);
    xd_vsip_vview_f *kernel = xd_vsip_vcreate_f(5,0);
    xd_vsip_vfill_f(1,kernel);
    xd_vsip_conv1d_f *plan1 = xd_vsip_conv1d_create_f(kernel,XD_VSIP_NONSYM,50,2,XD_VSIP_SUPPORT_FULL,1000000,XD_VSIP_ALG_SPACE);
    xd_vsip_convolve1d_f(plan1,b,c);
    //xd_vsip_vmul_f(a,b,c);
    int i = 0;
    int j = 0;
    int col = (int)xd_vsip_mgetcollength_f(a);
    int row = (int)xd_vsip_mgetrowlength_f(a);
    for (i = 0; i < row; i++)
    {
        for (j = 0;j < col;j++){
            //printf("%6.3f\t",xd_vsip_vget_f(a,i));
            printf("%6.3f\t",xd_vsip_mget_f(a,i,j));
        }
        printf("%d",i);
        printf("\n");
    }
    xd_vsip_malldestroy_f(a);
    xd_vsip_finalize((void*)0);
    return 0;
}