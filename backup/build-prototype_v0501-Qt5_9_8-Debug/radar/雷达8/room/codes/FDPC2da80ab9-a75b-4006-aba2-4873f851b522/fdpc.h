#include "fdpc.c"
void frequencyDomainPulseCompression(vsip_cmview_f *echo, vsip_cmview_f *echo_pulse,
                                     vsip_cmview_f *echo_fft, vsip_cmview_f *coeff_fft,
                                     vsip_cmview_f *pc0, vsip_cmview_f *pc, vsip_cmview_f *pc_fft,
                                     vsip_fftm_f *echoPlan, vsip_fftm_f *pcPlan);

