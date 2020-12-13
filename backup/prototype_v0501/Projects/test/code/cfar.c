#include <unistd.h>
#include "vsip.h"
#include "constant.h"

/**
 * 采用的是一维cfar，输入是经过mtd之后得到一维向量，针对每个值左右都进行求和，比较取大，再和当前位置进行
 * 比较大小，大取0，小取1
 * */
void constantFalseAlarmRate(vsip_mview_f *cfar_result, vsip_mview_f *mtd,
                            int protect_number, int reference_number,
                            int alpha, vsip_mview_f *mean_matrix, vsip_scalar_f temp)
{
    temp = vsip_mmeanval_f(vsip_msubview_f(mtd, 0, 0, 1, reference_number));
    vsip_mput_f(mean_matrix, 0, 0, temp);
    int i = 0;
    for (i = protect_number + reference_number; i < SampleNumber; i++)
    {
        temp += (vsip_mget_f(mtd, 0, i - protect_number) - vsip_mget_f(mtd, 0, i - protect_number - reference_number));
        vsip_mput_f(mean_matrix, 0, i - protect_number - reference_number, temp);
    }
    for (i = 0; i < SampleNumber; i++)
    {
        if (i < protect_number + reference_number) //如果i < p + r,直接取右边的值
        {
            temp = vsip_mget_f(mean_matrix, 0, i + protect_number);
        }
        else if (i > SampleNumber - protect_number - reference_number)
        //如果i > SampleNumber - p - r,直接取左边的值
        {
            temp = vsip_mget_f(mean_matrix, 0, i - protect_number - reference_number);
        }
        else //在中间的值取左右两边值中的较大值
        {
            temp = vsip_max_f(
                vsip_mget_f(mean_matrix, 0, i + protect_number),
                vsip_mget_f(mean_matrix, 0, i - protect_number - reference_number));
        }
        temp *= alpha; //乘以α系数
        //比较temp和在矩阵中对应位置的值，如果temp大的话，结果矩阵相应位置为1，反之为0
        if (temp > vsip_mget_f(mtd, 0, i))
        {
            vsip_mput_f(cfar_result, 0, i, 0.);
        }
        else
        {
            vsip_mput_f(cfar_result, 0, i, 1.);
        }
    }
}

// int main()
// {
//     vsip_mview_f *cfar_result = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_CONST);
//     vsip_mview_f *mtd = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_CONST);
//     vsip_mview_f *mean_matrix = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_CONST);
//     vsip_scalar_f temp;
//     int protect_number = 2;
//     int reference_number = 16;
//     int alpha = 2.4;
//     vsip_mfill_f((vsip_scalar_f)(1.0), mtd);
//     constantFalseAlarmRate(cfar_result,mtd,protect_number,reference_number,alpha,mean_matrix,temp);
//     int i = 0;
//     for (i = 0;i < vsip_mgetrowlength_f(cfar_result);i++){
//         printf("%f\t",vsip_mget_f(cfar_result,0,i));
//     }
// }