#include <vsip.h>
#include "constant.h"
#include <math.h>

//抗窄脉冲干扰

/**
 * pIngress 输入
 * pOutgress output
 * uRow 行
 * uCol 列
 * uRange 
*/
int NarrowPulseJam(vsip_cmview_f *pIngress,
					vsip_cmview_f *pOutgress,
				   unsigned int uRow, 
				   unsigned int uCol,
				   unsigned int uRange)
{
	vsip_cvview_f *pSubView, *pSubCol, *pSubRow;
	unsigned int i;
	float *pMagBuf, psQThold, dMinMag;
	vsip_block_f *pMagBlock;
	vsip_vview_f *pMagView0, *pMagView1, *pMagView2, *pMagView3, *pMagCol;

	unsigned int *replace_index, replace_sum, pindex;
	float pmem, pmem_i, temp_val;

	pMagBuf = (float *)malloc(4 * uRange * sizeof(float)); //分配内存
	pMagBlock = vsip_blockbind_f(pMagBuf, 4 * uRange, VSIP_MEM_NONE);		//将MagBuf绑定到pMagBlock中去
	vsip_blockadmit_f(pMagBlock, VSIP_TRUE);
	pMagView0 = vsip_vbind_f(pMagBlock, (vsip_offset)0, (vsip_stride)1, uRange); //pMagBlock的第一个子图
	pMagView1 = vsip_vbind_f(pMagBlock, uRange, (vsip_stride)1, uRange);		 //pMagBlock的第二个子图
	pMagView2 = vsip_vbind_f(pMagBlock, 2 * uRange, (vsip_stride)1, uRange);	 //pMagBlock的第三个子图
	pMagView3 = vsip_vbind_f(pMagBlock, 3 * uRange, (vsip_stride)1, uRange);	 //pMagBlock的第四个子图
	vsip_vfill_f((vsip_scalar_f)0.0, pMagView1);
	vsip_vfill_f((vsip_scalar_f)0.0, pMagView3); //第二个和第四个子图置为0

	pMagCol = vsip_vcreate_f(uRow, VSIP_MEM_NONE);

	psQThold = 100.0;
	dMinMag = 100.0;
	// printf("%f,%f\n",vsip_cmget_f(pIngress,0,0).r,vsip_cmget_f(pIngress,0,0).i);
	//按列求熵并与熵门限比较
	for (i = 0; i < uRow; i++)
	{											 //求熵
		pSubRow = vsip_cmrowview_f(pIngress, i); //获得第i行
		pSubView = vsip_cvsubview_f(pSubRow, 0, uRange);
		vsip_cvmag_f(pSubView, pMagView0);						//求模|xj|
		vsip_vadd_f(pMagView0, pMagView1, pMagView1);			//模列向和，pMagView1起始填0,sum(|xj|)
		vsip_vlog_f(pMagView0, pMagView2);						//ln(|xj|)
		vsip_vma_f(pMagView0, pMagView2, pMagView3, pMagView3); /*pMagView3起始填0，sum(|xj|*ln(|xj|)*/
		vsip_cvdestroy_f(pSubView);
		vsip_cvdestroy_f(pSubRow);
	}
	vsip_vlog_f(pMagView1, pMagView0);			  //ln(sum(|xj|))
	vsip_vdiv_f(pMagView3, pMagView1, pMagView1); //sum(|xj|*ln(|xj|)/sum(|xj|)
	vsip_vsub_f(pMagView0, pMagView1, pMagView0); //ln(sum(|xj|))-sum(|xj|*ln(|xj|)/sum(|xj|)

	// printf("%f,%f\n",vsip_cmget_f(pIngress,0,0).r,vsip_cmget_f(pIngress,0,0).i);
	/*熵门限比较*/
	for (i = 0; i < uRange; i++) //？
	{
		if (pMagBuf[i] <= psQThold)
		{
			pSubCol = vsip_cmcolview_f(pIngress, i);   //获得第i列
			vsip_cvmag_f(pSubCol, pMagCol);			   //列向模
			vsip_crvdiv_f(pSubCol, pMagCol, pSubCol);  //xj=xj/|xj|
			vsip_rscvmul_f(dMinMag, pSubCol, pSubCol); //xj*min(|xj|)
			vsip_cvdestroy_f(pSubCol);
		}
	}

	int j = 0;
	for (i = 0;i < PulseNumber;i++){
		for(j = 0;j < SampleNumber;j++){
			vsip_cmput_f(pOutgress,i,j,vsip_cmget_f(pIngress,i,j));
		}
	}
	// printf("%f,%f\n",vsip_cmget_f(pIngress,0,0).r,vsip_cmget_f(pIngress,0,0).i);
	vsip_valldestroy_f(pMagCol);
	vsip_vdestroy_f(pMagView0);
	vsip_vdestroy_f(pMagView1);
	vsip_vdestroy_f(pMagView2);
	vsip_vdestroy_f(pMagView3);
	vsip_blockrelease_f(pMagBlock, VSIP_TRUE);
	vsip_blockdestroy_f(pMagBlock);
	free(pMagBuf);
	return 0;
}

// int main(){
// 	vsip_cmview_f *in = vsip_cmcreate_f(16,1024,VSIP_ROW,VSIP_MEM_CONST);
// 	vsip_cmfill_f(vsip_cmplx_f(1,0.1),in);
// 	NarrowPulseJam(in,16,1024,1024);
// 	int i;
// 	for (i = 0;i < 16;i++){
// 		printf("%f\t%f\n",vsip_cmget_f(in,i,15).r,vsip_cmget_f(in,i,16).r);
// 	}
// }
