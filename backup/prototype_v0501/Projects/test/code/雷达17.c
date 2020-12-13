#include "rsplBspHead.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "RspMain.h"

	struct timeval t1, t2, t3; //实验计时用//time

    int i = 0;
    double count = 0.0; //time
    int number = 4;    //数据的组数，一组为9600

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
	for (i = 0; i < number; i++)
	{


		// INPUT函数调用

		readData("./data/INPUT.txt", echo, i * SampleNumber * PulseNumber, 0);
        gettimeofday(&t1, NULL);
		// MTD函数调用

		movingTargetDetection(mtd, pc, buff, buff_fft, win, buff_plan, max, tempm);
        writeData_1("./data/MTD.txt",mtd);
		// CFAR函数调用

		constantFalseAlarmRate(cfar_result, mtd, protect_number, reference_number, alpha, mean_matrix, temp);
        gettimeofday(&t2, NULL);
        printf("speed time is:%f(ms)\n", (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0); //time
        writeData_1("./data/CFAR.txt", cfar_result);
		// OUTPUT函数调用

		writeData_1("./data/OUTPUT.txt", cfar_result);
        count += (double)(t2.tv_sec * 1e6 + t2.tv_usec) / 1000.0 - (double)(t1.tv_sec * 1e6 + t1.tv_usec) / 1000.0; //time

