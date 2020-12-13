#include "fftw3.h"
#include "complexOperation.h"
#include <iostream>
using namespace std;

#define PulseNumber 8
#define SampleNumber 2700

void movingTargetDetection(fftw_complex **mtd,fftw_complex **pc,fftw_complex *buff,fftw_complex *buff_fft,fftw_plan buffplan){
    int i,j;
    for (i = 0;i < PulseNumber;i++){
        for (j = 0;j < SampleNumber;j++){
            mtd[i][j][0] = mtd[i][j][1] = 0;
        }
    }
    //cout << 4 << endl;
    for (i = 0; i < SampleNumber;i++){
        for (j = 0;j < PulseNumber;j++){
            buff[j][0] = pc[j][i][0];
            buff[j][1] = pc[j][i][1];
    
        }
        //cout << 6 << endl;
        fftw_execute(buffplan);
        //cout << 7 << endl;
        for (j = 0;j < PulseNumber;j++){
            mtd[j][i][0] = buff_fft[j][0];
            mtd[j][i][1] = buff_fft[j][1];
        
        }
        
    }
    //cout << 5 << endl;
}