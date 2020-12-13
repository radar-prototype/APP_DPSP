#include "mtd.c"
void movingTargetDetection(vsip_mview_f *mtd, vsip_cmview_f *pc,
                           vsip_cmview_f *buff, vsip_cmview_f *buff_fft,
                           vsip_mview_f *win, vsip_fftm_f *buffPlan,
                           vsip_scalar_f max, vsip_scalar_f temp);