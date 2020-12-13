#include <vsip.h>
#include "constant.h"
#include "ham.c"
/**
 * 动目标检测,区分不同速度的目标，有测速作用
 * mtd:输出
 * pc:输入
 * buff:截取输入的每一个脉冲
 * buff_fft:buff傅里叶变换结果
 * buffPlan:buff傅里叶变换
 * */
void movingTargetDetection(vsip_mview_f *mtd, vsip_cmview_f *pc,
                           vsip_cmview_f *buff, vsip_cmview_f *buff_fft,
                           vsip_mview_f *win, vsip_fftm_f *buffPlan,
                           vsip_scalar_f max, vsip_scalar_f temp)
{
    int i, j;
    for (i = 0; i < SampleNumber; i++)
    {
        max = VSIP_MIN_SCALAR_F;
        buff = vsip_cmsubview_f(pc, 0, i, PulseNumber, 1);//取列操作
        vsip_rcmmul_f(win, buff, buff);//和窗口函数点乘 汉明窗
        vsip_ccfftmop_f(buffPlan, buff, buff_fft);// fft运算
        for (j = 0; j < PulseNumber; j++)
        {//取最大值，max 初始为VSIP_MIN_SCALAR_F
            temp = vsip_sqrt_f(
                vsip_pow_f(vsip_cmget_f(buff_fft, j, 0).r, 2) +
                vsip_pow_f(vsip_cmget_f(buff_fft, j, 0).i, 2));
            if (max < temp)
            {
                max = temp;
            }
        }
        vsip_mput_f(mtd,0,i,max);//将最大值放入结果的相应位置上面
    }
}