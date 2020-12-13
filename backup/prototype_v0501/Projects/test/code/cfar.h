#include "cfar.c"
void constantFalseAlarmRate(vsip_mview_f *cfar_result, vsip_mview_f *mtd,
                            int protect_number, int reference_number,
                            int alpha, vsip_mview_f *mean_matrix, vsip_scalar_f temp);