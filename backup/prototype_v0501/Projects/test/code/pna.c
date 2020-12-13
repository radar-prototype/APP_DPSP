#include <vsip.h>
#include <math.h>
#include "pRcb.h"

#define LIGHT_SPEED 10
#define fFs 1
#define g_uAjcLevel 1.0

/**
 * pIngress : 输入
 * pOutgress : output
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
			   float *pNcaMag)
{
	vsip_cvview_f *pLineSub;
	vsip_cblock_f *pBlock;
	/*抗同频异步干扰，非相参使用变量*/
	vsip_block_f *pMagBlock;
	vsip_block_bl *pBoolBlock;
	vsip_vview_f *pMagView, *pAcjLevelView;
	vsip_vview_bl *pBoolView0, *pBoolView1, *pBoolView2;
	float *pMagBuf;
	vsip_bool *pBoolBuf, *pBool0, *pBool1, *pBool2;
	unsigned int uId, uId0, uId1, uId2, uMagId, i, j;
	vsip_cscalar_f fNoise;
	vsip_scalar_f fNoiseMag, fScale;
	/*平台运动补偿，平台径向速度方向（有符号数，相对正，背离负）决定补偿的正负*/
	// TX_CODE_DSP *pCode;
	float Ts, *PmcRealBase, *PmcImagBase;
	vsip_cblock_f *PmcBlock;
	vsip_cvview_f *PmcView;
	vsip_vview_f *pMiddView, *pAngView;
	vsip_scalar_f angle;
	vsip_scalar_f t_start;
	float Fd = 2.0 * (float)pRcb->sRadialVol / 100.0 * (float)pRcb->sRf * 1e6 / LIGHT_SPEED;
	// 1.将LIGHT_SPEED改成参数light_speed
	float tmp = Fd * 2.0 * VSIP_PI; //2*pi*fd

	// pCode = CodeQuery(pRcb->sCode);
	// if (pCode == NULL)
	// 	return -1; //2014.6.11 软件测试
	Ts = 1 / fFs;
	// 2.将fFs改成参数

	/*平台运动补偿变量生成*/
	PmcRealBase = (float *)malloc(uRange * sizeof(float));
	PmcImagBase = (float *)malloc(uRange * sizeof(float));

	PmcBlock = vsip_cblockbind_f(PmcRealBase, PmcImagBase, uRange, VSIP_MEM_NONE);
	vsip_cblockadmit_f(PmcBlock, VSIP_TRUE);
	PmcView = vsip_cvbind_f(PmcBlock, (vsip_offset)0, (vsip_stride)1, uRange);
	pMiddView = vsip_vcreate_f(uRange, VSIP_MEM_NONE);

	t_start = 0.0;							 //(float)pRcb->sPGLen * Ts
	vsip_vramp_f(t_start, Ts, pMiddView);	//t = 0:ts:pw
	vsip_svmul_f(tmp, pMiddView, pMiddView); //a = 2*pi*fd*t

	//构建NCA和ACJ输出矢量
	pMagBuf = (float *)malloc(2 * uRange * sizeof(float));
	pBoolBuf = (vsip_bool *)malloc(3 * uRange * sizeof(vsip_bool));

	uMagId = 0;
	uId0 = 0;
	uId1 = 1;
	uId2 = 2;

	pMagBlock = vsip_blockbind_f(pMagBuf, 2 * uRange, VSIP_MEM_NONE);
	vsip_blockadmit_f(pMagBlock, VSIP_TRUE);
	pMagView = vsip_vbind_f(pMagBlock, (vsip_offset)0, (vsip_stride)1, uRange);

	pBoolBlock = vsip_blockbind_bl(pBoolBuf, 3 * uRange, VSIP_MEM_NONE);
	vsip_blockadmit_bl(pBoolBlock, VSIP_TRUE);
	pBoolView0 = vsip_vbind_bl(pBoolBlock, uId0 * uRange, (vsip_stride)1, uRange);
	pBoolView1 = vsip_vbind_bl(pBoolBlock, uId1 * uRange, (vsip_stride)1, uRange);
	pBoolView2 = vsip_vbind_bl(pBoolBlock, uId2 * uRange, (vsip_stride)1, uRange);

	pAcjLevelView = vsip_vcreate_f(uRange, VSIP_MEM_NONE);
	vsip_vfill_f((vsip_scalar_f)g_uAjcLevel, pAcjLevelView); //同频异步干扰门限
	// 3.将g_uAjcLevel_DSP 改成变参数

	//fNoise = vsip_cmplx_f(pRcb->sNsLevel, pRcb->sNsLevel);//噪声电平
	fNoise = vsip_cmplx_f((vsip_scalar_f)1e-6, (vsip_scalar_f)1e-6); //噪声电平
	// 4.将EPSOLO改成 1e-6

	fNoiseMag = vsip_cmag_f(fNoise);

	//构建脉压环境
	fScale = 1.0 / sqrt((float)uCol);
	pBlock = vsip_cmgetblock_f(pIngress);
	pLineSub = vsip_cvbind_f(pBlock, (vsip_offset)0, (vsip_stride)1, uRange);

	pAngView = vsip_vcreate_f(uRange, VSIP_MEM_NONE);

	//非相参积累
	if (bNcaEn)
	{
		for (i = 0; i < uRow; i++) //按行
		{
			//Get Line i
			vsip_cvputoffset_f(pLineSub, i * uCol * 2);

			/*平台运动补偿*/
			vsip_vfill_f((vsip_scalar_f)2.0, pAngView);
			angle = 2.0 * VSIP_PI * Fd * i * (float)pRcb->sPrt / 5.0 * (1e-6);
			vsip_svadd_f(angle, pMiddView, pAngView); //a = 2*pi*fd*t+phi
			vsip_veuler_f(pAngView, PmcView);		  //w = cos(a) + j*sin(a)


			vsip_cvmul_f(pLineSub, PmcView, pLineSub);
			//////////////////////////////////////////////////

			vsip_vputoffset_f(pMagView, uMagId * uRange);
			vsip_vputoffset_bl(pBoolView0, uId0 * uRange);
			vsip_vputoffset_bl(pBoolView1, uId1 * uRange);
			vsip_vputoffset_bl(pBoolView2, uId2 * uRange);

			//uMagPreId = uMagId;
			uMagId = (uMagId + 1) % 2; //i偶数1，i奇数0

			vsip_cvmag_f(pLineSub, pMagView); //求模

			if (i == 0)
			{
				vsip_vlge_f(pMagView, pAcjLevelView, pBoolView0);
			}
			else if (i == 1)
			{
				vsip_vlge_f(pMagView, pAcjLevelView, pBoolView1);
				pBool0 = pBoolBuf;
				pBool1 = pBoolBuf + uRange;
				for (j = 0; j < uRange; j++)
				{
					if ((pBool0[j]) && (!pBool1[j])) //1,0
					{
						vsip_cmput_f(pIngress, 0, j, fNoise);
						pNcaMag[j] = fNoiseMag;
					}
					else
					{
						pNcaMag[j] = pMagBuf[j];
					}
				}
			}
			else
			{
				vsip_vlge_f(pMagView, pAcjLevelView, pBoolView2); //门限(比较)
				pBool0 = pBoolBuf + uId0 * uRange;
				pBool1 = pBoolBuf + uId1 * uRange;
				pBool2 = pBoolBuf + uId2 * uRange;

				for (j = 0; j < uRange; j++)
				{ //0,1,0
					if ((!pBool0[j]) && (pBool1[j]) && (!pBool2[j]))
					{
						vsip_cmput_f(pIngress, i - 1, j, fNoise);
						pNcaMag[j] += fNoiseMag;
					}
					else
					{
						pNcaMag[j] += pMagBuf[j + uMagId * uRange];
					}
				}

				uId = uId0;
				uId0 = uId1;
				uId1 = uId2;
				uId2 = uId;
			}
		}
		if (uRow > 2) //2014.6.11 软件测试
		{
			uMagId = (uMagId + 1) % 2; //i偶数1，i奇数0
			for (j = 0; j < uRange; j++)
			{
				if ((!pBool1[j]) && (pBool2[j])) //0,1
				{
					vsip_cmput_f(pIngress, i - 1, j, fNoise);
					pNcaMag[j] += fNoiseMag;
				}
				else
				{
					pNcaMag[j] += pMagBuf[j + uMagId * uRange];
				}
			} //end acj
		}
	}
	else
	{
		for (i = 0; i < uRow; i++) //按行
		{
			//Get Line i
			vsip_cvputoffset_f(pLineSub, i * uCol * 2);

			/*平台运动补偿*/
			vsip_vfill_f((vsip_scalar_f)0.0, pAngView);
			angle = 2.0 * VSIP_PI * Fd * i * (float)pRcb->sPrt / 5.0 * (1e-6);
			vsip_svadd_f(angle, pMiddView, pAngView); //a = 2*pi*fd*t+phi
			vsip_veuler_f(pAngView, PmcView);		  //w = cos(a) + j*sin(a)
			vsip_cvmul_f(pLineSub, PmcView, pLineSub);

			//////////////////////////////////////////////////
			{
				vsip_vputoffset_f(pMagView, uMagId * uRange);
				vsip_vputoffset_bl(pBoolView0, uId0 * uRange);
				vsip_vputoffset_bl(pBoolView1, uId1 * uRange);
				vsip_vputoffset_bl(pBoolView2, uId2 * uRange);

				//uMagPreId = uMagId;
				uMagId = (uMagId + 1) % 2; //i偶数1，i奇数0

				vsip_cvmag_f(pLineSub, pMagView); //求模

				if (i == 0)
				{
					vsip_vlge_f(pMagView, pAcjLevelView, pBoolView0);
				}
				else if (i == 1)
				{
					vsip_vlge_f(pMagView, pAcjLevelView, pBoolView1);
					pBool0 = pBoolBuf;
					pBool1 = pBoolBuf + uRange;
					for (j = 0; j < uRange; j++)
					{
						if ((pBool0[j]) && (!pBool1[j])) //1,0
						{
							vsip_cmput_f(pIngress, 0, j, fNoise);
						}
					}
				}
				else
				{
					vsip_vlge_f(pMagView, pAcjLevelView, pBoolView2); //门限(比较)
					pBool0 = pBoolBuf + uId0 * uRange;
					pBool1 = pBoolBuf + uId1 * uRange;
					pBool2 = pBoolBuf + uId2 * uRange;
					for (j = 0; j < uRange; j++)
					{ //0,1,0
						if ((!pBool0[j]) && (pBool1[j]) && (!pBool2[j]))
						{
							vsip_cmput_f(pIngress, i - 1, j, fNoise);
						}
					}
					uId = uId0;
					uId0 = uId1;
					uId1 = uId2;
					uId2 = uId;
				}
			}
		}

		if (uRow > 2) //2014.6.11 软件测试
		{
			for (j = 0; j < uRange; j++)
			{
				if ((!pBool1[j]) && (pBool2[j])) //0,1
				{
					vsip_cmput_f(pIngress, i - 1, j, fNoise);
				}
			} //end acj
		}
	}

	for (i = 0;i < PulseNumber;i++){
		for(j = 0;j < SampleNumber;j++){
			vsip_cmput_f(pOutgress,i,j,vsip_cmget_f(pIngress,i,j));
		}
	}
	/*抗同频异步干扰变量释放*/
	{
		vsip_valldestroy_f(pAcjLevelView);
		vsip_vdestroy_f(pMagView);
		vsip_vdestroy_bl(pBoolView0);
		vsip_vdestroy_bl(pBoolView1);
		vsip_vdestroy_bl(pBoolView2);
		vsip_blockrelease_f(pMagBlock, VSIP_TRUE);
		vsip_blockdestroy_f(pMagBlock);
		vsip_blockrelease_bl(pBoolBlock, VSIP_TRUE);
		vsip_blockdestroy_bl(pBoolBlock);

		vsip_cvdestroy_f(pLineSub);
		free(pMagBuf);
		free(pBoolBuf);
		/*平台运动补偿变量释放*/
		vsip_valldestroy_f(pMiddView);
		vsip_valldestroy_f(pAngView);
		vsip_cvdestroy_f(PmcView);
		vsip_cblockrelease_f(PmcBlock, VSIP_TRUE, &PmcRealBase, &PmcImagBase);
		vsip_cblockdestroy_f(PmcBlock);
		free(PmcRealBase);
		free(PmcImagBase);
	}
	return 0;
}
