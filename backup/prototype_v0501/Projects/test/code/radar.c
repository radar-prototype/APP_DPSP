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
    vsip_init((void *)0);
    //雷达中用到的数据初始化
    vsip_cmview_f *coeff = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_cmview_f *echo = vsip_cmcreate_f(PulseNumber, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_fftm_f *coeff_plan = vsip_ccfftmop_create_f(1, SampleNumber, 1, VSIP_FFT_FWD, VSIP_ROW, 1, VSIP_ALG_SPACE);
    vsip_cmview_f *coeff_fft = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    readData("./coeff.txt", coeff, 0, 0);
    vsip_ccfftmop_f(coeff_plan, coeff, coeff_fft);

    
    vsip_cmview_f *echo_pulse = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_cmview_f *echo_fft = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_cmview_f *pc_fft = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_cmview_f *pc0 = vsip_cmcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_fftm_f *pc_plan = vsip_ccfftmop_create_f(1, SampleNumber, 1, VSIP_FFT_INV, VSIP_ROW, 10, VSIP_ALG_NOISE);
    vsip_cmview_f *pc = vsip_cmcreate_f(PulseNumber, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_fftm_f *echo_plan = vsip_ccfftmop_create_f(1, SampleNumber, 1, VSIP_FFT_FWD, VSIP_ROW, 10, VSIP_ALG_NOISE);

    vsip_mview_f *mtd = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_CONST);
    vsip_cmview_f *buff = vsip_cmcreate_f(PulseNumber, 1, VSIP_ROW, VSIP_MEM_NONE);
    vsip_cmview_f *buff_fft = vsip_cmcreate_f(PulseNumber, 1, VSIP_ROW, VSIP_MEM_NONE);
    vsip_fftm_f *buff_plan = vsip_ccfftmop_create_f(PulseNumber, 1, 1, VSIP_FFT_FWD, VSIP_COL, 1, VSIP_ALG_NOISE);
    vsip_mview_f *win = vsip_mcreate_f(PulseNumber, 1, VSIP_ROW, VSIP_MEM_CONST);
    GenHamWindow(PulseNumber, win);
    vsip_scalar_f max = VSIP_MIN_SCALAR_F;
    vsip_scalar_f tempm = 0.;

    vsip_scalar_f temp;
    int protect_number = 3;
    int reference_number = 50;
    int alpha = 2;
    vsip_mview_f *cfar_result = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_mview_f *noise_gate = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_NONE);
    vsip_mview_f *mean_matrix = vsip_mcreate_f(1, SampleNumber, VSIP_ROW, VSIP_MEM_CONST);
    
    RCB_DSP *pRcb = (RCB_DSP*)malloc(sizeof(RCB_DSP) * 1);
	pRcb->sPrt = 2;
	pRcb->sRf = 1;
	pRcb->sRadialVol = 1;
	unsigned int uRow = 16;
	unsigned int uCol = 10;
	unsigned int uRange = 1024;
	vsip_bool bNcaEn = VSIP_TRUE;
	float *pNcaMag = (float *)malloc(sizeof(float) * uRange);
	
    struct timeval t1, t2, t3; //实验计时用//time

    int i = 0;
    double count = 0.0; //time
    int number = 4;    //数据的组数，一组为9600
	vsip_cmview_f *pna_out = vsip_cmcreate_f(PulseNumber,SampleNumber,VSIP_ROW,VSIP_MEM_CONST);
	vsip_cmview_f *npj_out = vsip_cmcreate_f(PulseNumber,SampleNumber,VSIP_ROW,VSIP_MEM_CONST);    
    for (i = 0; i < number; i++)
    {

        readData("./data/INPUT.txt", echo, i * SampleNumber * PulseNumber, 0);
        gettimeofday(&t1, NULL);
        PC_NCA_AJC(echo,pna_out, pRcb, uRow, uCol, uRange, bNcaEn, pNcaMag);
        writeData("./data/PNA.txt",echo);
        NarrowPulseJam(pna_out,npj_out,PulseNumber,SampleNumber,SampleNumber);
        writeData("./data/NPJ.txt",echo);
        frequencyDomainPulseCompression(npj_out, echo_pulse, echo_fft, coeff_fft, pc0, pc, pc_fft, echo_plan, pc_plan);
        // frequencyDomainPulseCompression1(echo, echo_pulse, echo_fft,
        //                                  coeff_fft, pc0, pc, pc_fft,
        //                                  echo_plan, pc_plan, fdpc_in_temp,
        //                                  fdpc_out_temp, fdpc_plan, 1);
        writeData("./data/FDPC.txt",pc);
        movingTargetDetection(mtd, pc, buff, buff_fft, win, buff_plan, max, tempm);
        writeData_1("./data/MTD.txt",mtd);
        constantFalseAlarmRate(cfar_result, mtd, protect_number, reference_number, alpha, mean_matrix, temp);
        gettimeofday(&t2, NULL);
        printf("speed time is:%f(ms)\n", (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0); //time
        writeData_1("./data/CFAR.txt", cfar_result);
		writeData_1("./data/OUTPUT.txt", cfar_result);
        count += (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0; //time
    }

    printf("the avgerage time: %f\n", count / number);
	vsip_finalize((void *)0);
	return 0;
}
