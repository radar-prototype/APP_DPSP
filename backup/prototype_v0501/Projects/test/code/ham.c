#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vsip.h"

#define TPI 6.28318530717959
#define FRAMESIZE 128

/* GenHamWindow: generate precomputed Hamming window function */
void GenHamWindow(int frameSize, vsip_mview_f *hamWin)
{
    int i;
    float a;

    a = (float)TPI / (frameSize - 1);
    for (i = 1; i <= frameSize; i++){
        vsip_mput_f(hamWin,0,i,(float)(0.54 - 0.46 * cos(a * (i - 1))));
    }
    vsip_mputoffset_f(hamWin,1);
}

// int main()
// {
//     int i;
//     vsip_mview_f *s = vsip_mcreate_f(1,FRAMESIZE+1,VSIP_ROW,VSIP_MEM_CONST);
//     GenHamWindow(FRAMESIZE, s);
//     for (i = 0; i < FRAMESIZE; i++)
//     {
//         printf("%f,",vsip_mget_f(s,0,i));
//     }
    
//     return 0;
// }