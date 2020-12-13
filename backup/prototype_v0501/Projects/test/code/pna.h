#include "pna.c"
/**
 * pIngress : 输入
 * pRcb : 结构体,用到了结构体中的一些系数
 * uRow : 行
 * uCol : 列
 * uRange : 
 * bNcaEn : 是否相参积累
 * pNcaMag : 输出 长度为 uRange
*/
int PC_NCA_AJC(vsip_cmview_f *pIngress,vsip_cmview_f *pOutgress,
			   RCB_DSP *pRcb,
			   unsigned int uRow,
			   unsigned int uCol,
			   unsigned int uRange,
			   vsip_bool bNcaEn,
			   float *pNcaMag);
