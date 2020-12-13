#include "cfar.h"
#include "fdpc.h"
#include "mtd.h"
#include "readFile.h"
#include "writeFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "npj.h"
#include "pna.h"


int main()
{
    xd_vsip_init((void *)0);
    float *input = (float*)malloc(sizeof(float) * PulseNumber * SampleNumber);
    int i = 0;
    for (i = 0; i < PulseNumber * SampleNumber; i++)
    {
        input[i] = i;
    }
    xd_vsip_block_f *cblock = xd_vsip_blockbind_f(input,PulseNumber * SampleNumber,XD_VSIP_MEM_CONST);
    xd_vsip_cmview_f *m_view = xd_vsip_mbind_f(cblock,0,SampleNumber,PulseNumber,1,SampleNumber);
    xd_vsip_cvview_f *v_view = xd_vsip_vbind_f(cblock,0,1,SampleNumber);
    xd_vsip_vview_f *temp;
    struct timeval t1, t2, t3; //实验计时用//time
    gettimeofday(&t1, NULL);
    for (i = 0; i < xd_vsip_mgetrowlength_f; i++)
    {
        //直接在float
        temp = xd_vsip_mputoffset_f(m_view,i*SampleNumber);
        // temp = xd_vsip_msubview_f(m_view,0,i*SampleNumber,1,SampleNumber);
        // xd_vsip_vmul_f(temp,v_view,temp);
    }
    gettimeofday(&t2, NULL);
    for (i = 0; i < xd_vsip_mgetrowlength_f; i++)
    {
        //直接在VSIPL上面进行操作的
        // temp = xd_vsip_mputoffset_f(m_view,i*SampleNumber);
        temp = xd_vsip_msubview_f(m_view,0,i*SampleNumber,1,SampleNumber);
        // xd_vsip_vmul_f(temp,v_view,temp);
    }
    gettimeofday(&t3, NULL);

    printf("在数据格式为16 * 1024的矩阵中,使用原生的移位操作和改进之后移位操作对比\n");
    printf("改进之前的时间为:%f(ms)\n", (double)(t3.tv_sec * 1e6 + t3.tv_usec) / 1000.0 - (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0); //time
    printf("改进之后的时间为:%f(ms)\n", (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0); //time
        
    xd_vsip_finalize((void *)0);
}