#include "cfar.h"
#include "fdpc.h"
#include "mtd.h"
#include "complexOperation.cpp"
#include "readFile.h"
#include "writeFile.h"
#include "time.h"
#include "omp.h"
#include <iostream>

using namespace std;


int main(){
    int i,j,k,m,n,leftSectionIndex,rightSectionIndex = 0;
    int reference_num = 50;
    int protect_num = 3;
    int alpha = 2;

    double distance_result = 0.0;
	double velocity_result = 0.0;
	double PRT = 270.0e-6;
	double PRF = 1 / PRT;
	double C = 3.0e8;
	double RF = 3.140e9 / 2;
	double Lambda = 6 / 31.4;

    fftw_complex* pc_freq1 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (SampleNumber * PulseNumber));
	fftw_complex* pc_freq0 = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
	fftw_complex* echo_Pulse = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
	fftw_complex* echo_fft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
	fftw_complex* coeff_fft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
	fftw_complex* pc_fft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
	fftw_complex* buff = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PulseNumber);
	fftw_complex* buff_fft = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PulseNumber);
	fftw_plan fftwPlan;
	fftw_plan fftwPlan2;
	fftw_plan fftwPlan3;
	fftw_complex** pc = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * PulseNumber);
	fftw_complex** mtd = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * PulseNumber);
	fftw_complex** cfar_result = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * PulseNumber);
	fftw_complex** noise_gate = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * PulseNumber);
	fftw_complex* peaks = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 54);
    for (i = 0;i < PulseNumber;i++){
        pc[i] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SampleNumber);
        mtd[i] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SampleNumber);
        cfar_result[i] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SampleNumber);
        noise_gate[i] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * SampleNumber);
    }
    



    int length = dataLength("./coeff.txt");
    fftw_complex* coeff = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
    readData("./coeff.txt",coeff,0,4096,0);
    for (int i = length;i < 4096;i++){
        coeff[i][0] = coeff[i][1] = 0;
    }
    
    fftwPlan = fftw_plan_dft_1d(4096,coeff,coeff_fft,FFTW_FORWARD,FFTW_ESTIMATE);
    fftw_execute(fftwPlan);
    fftwPlan = fftw_plan_dft_1d(4096,echo_Pulse,echo_fft,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwPlan2 = fftw_plan_dft_1d(4096,pc_fft,pc_freq0,FFTW_BACKWARD,FFTW_ESTIMATE);
    fftwPlan3 = fftw_plan_dft_1d(PulseNumber,buff,buff_fft,FFTW_FORWARD,FFTW_ESTIMATE);

    time_t start,finish;
    start = time(NULL);
    double_t start_time = omp_get_wtime();
    #pragma omp parallel for
    for (int count = 0;count < 10;count++){
        fftw_complex* echoArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * PulseNumber * SampleNumber);
        int row_count = 0;
        readData("./Echoes.txt",echoArray,row_count,row_count + 4096,1);
        frequencyDomainPulseCompression(echoArray,echo_Pulse,pc_fft,echo_fft,coeff_fft,fftwPlan,fftwPlan2,pc_freq0,pc_freq1,pc);
        movingTargetDetection(mtd,pc,buff,buff_fft,fftwPlan3);
        constantFalseAlarmRate(cfar_result,noise_gate,mtd,protect_num,reference_num,alpha);
        findMaxRowAndColumn(cfar_result,peaks,10,PRF,Lambda);
        row_count += 4096;
        }
    finish = time(NULL);
    double_t end_time = omp_get_wtime();
    printf("%fs\n",(end_time-start_time));
    writeData("cfar_result.txt",cfar_result);
}
