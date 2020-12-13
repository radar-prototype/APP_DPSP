#include <vsip.h>
#include <stdio.h>
#include "pna.h"
typedef struct  temp
{
    int a;
    vsip_cmview_f *b;
}t_t;

int main(){
    RCB_DSP *pRcb = (RCB_DSP*)malloc(sizeof(RCB_DSP) * 1);
    pRcb->sPrt = 2;
    vsip_cmview_f *a = vsip_cmcreate_f(10,10,VSIP_ROW,VSIP_MEM_CONST);
    t_t *temp;
    printf("%ld,%ld,%d\n",sizeof(&temp),sizeof(vsip_cmview_f*),pRcb->sPrt);
}