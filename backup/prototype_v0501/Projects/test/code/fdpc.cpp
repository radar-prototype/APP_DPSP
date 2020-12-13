#include "fftw3.h"
#include "complexOperation.h"
#include <iostream>
using namespace std;

#define PulseNumber 8
#define SampleNumber 2700
void frequencyDomainPulseCompression(fftw_complex* echo,fftw_complex* echo_pulse,fftw_complex* pc_fft,
    fftw_complex* echo_fft,fftw_complex* coeff_fft,fftw_plan echo_plan,fftw_plan pcplan,
    fftw_complex* pc_freq0,fftw_complex* pc_freq1,fftw_complex** pc)
{
    int left_section_index,right_section_index;
    for (int i = 1;i <= PulseNumber;i++){
        left_section_index = (i - 1) * SampleNumber;
        right_section_index = (i - 1) * SampleNumber + SampleNumber - 1;
        sliceData(echo,left_section_index,right_section_index,echo_pulse,0);
        for (int j = SampleNumber;j < 4096;j++){
            echo_pulse[j][0] = echo_pulse[j][1] = 0.0;
        }
        fftw_execute(echo_plan);
        multiplication(pc_fft,echo_fft,coeff_fft,4096);
        fftw_execute(pcplan);
        scaleValue(pc_freq0,4096,4096);
        sliceData(pc_freq0,539,3298,pc_freq1,left_section_index);
    }

    for (int i = 1; i <= PulseNumber; i++)
    {
        left_section_index = (i - 1) * SampleNumber;
        for (int j = 0;j < SampleNumber;j++){
            pc[i - 1][j][0] = pc_freq1[left_section_index][0];
            pc[i - 1][j][1] = pc_freq1[left_section_index][1];
            left_section_index++;
        }
    }
    
}