#include "fftw3.h"
#include <iostream>
#include "complexOperation.h"
using namespace std;

#define PulseNumber 8
#define SampleNumber 2700
void constantFalseAlarmRate(fftw_complex** cfar_result,fftw_complex** noise_gate,fftw_complex **mtd,
    int protect_num,int reference_num,int alpha)
{
    int i,j,m,n;
    // initialize the matrix cafr_result and noise_gate
    for (i = 0;i < PulseNumber;i++)
    {
        for (j = 0; j < SampleNumber; j++)
        {
            cfar_result[i][j][0] = cfar_result[i][j][1] = noise_gate[i][j][0] = noise_gate[i][j][0] = noise_gate[i][j][1] = 0;
    
        }
    }

    for (m = 0;m < PulseNumber;m++){
        for (n = 99;n < 2600;n++){
            double real_part1, real_part2, imagnary_part1, imagnary_part2;
            complexMeanValue(mtd,m,n-protect_num-reference_num,n-protect_num,&real_part1,&imagnary_part1);
            complexMeanValue(mtd,m,n+protect_num,n+protect_num+reference_num,&real_part2,&imagnary_part2);
            real_part2 /= 2.0;
            imagnary_part2 /= 2.0;
            noise_gate[m][n][0] = alpha * (real_part1 + real_part2);
            noise_gate[m][n][1] = alpha * (imagnary_part1 + imagnary_part2);
            if (mtd[m][n][0] >= noise_gate[m][n][0])
            {
                cfar_result[m][n][0] = mtd[m][n][0];
                cfar_result[m][n][1] = mtd[m][n][1];   
            }
            
        }
    }
}