#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include "fftw3.h"
using namespace std;

#define PulseNumber 8
#define SampleNumber 2700

//  result is the final result, array1 and array2 are the two arrays that will be calculated, length is the arrays' length
void multiplication(fftw_complex* result,fftw_complex *array1,fftw_complex *array2,int length){
    int i = 0;
    for (int i = 0 ; i < length ; i++){
       result[i][0] = array1[i][0] * array2[i][0] - array1[i][1] * array2[i][1];
       result[i][1] = array1[i][1] * array2[i][0] + array1[i][0] * array2[i][1];
    }
}

// scale the array by using the parameter scale
void scaleValue(fftw_complex *result,int scale,int length){
    int i = 0;
    for (int i = 0;i < length;i++){
        result[i][0] = (double)result[i][0] / scale;
        result[i][1] = (double)result[i][1] / scale;
    }
}

// get the mean value of the array where parameter rp is the real part and ip is the imaginary part
void complexMeanValue(fftw_complex **result,int index,int leftIndex,int rightIndex,double *rp,double *ip){
    double real_part = 0;
    double imaginary_part = 0;
    for (int i = leftIndex;i < rightIndex;i++){
        real_part += result[index][i][0];
        imaginary_part += result[index][i][1];
    }
    *rp = real_part / (rightIndex - leftIndex + 1);
    *ip = imaginary_part / (rightIndex - leftIndex + 1);
}
// slice complex_array from leftindex to rightindex, and parameter result store the final rsult, inLeftIndex
// is the index of the array result
void sliceData(fftw_complex *complex_array,int leftIndex,int rightIndex,fftw_complex *result,int inLeftIndex){
    for (; leftIndex < rightIndex;leftIndex++,inLeftIndex++){
        result[inLeftIndex][0] = complex_array[leftIndex][0];
        result[inLeftIndex][1] = complex_array[leftIndex][1];
    }
}
void findMaxRowAndColumn(fftw_complex** cfar_result, fftw_complex* peaks, int loop, double PRF, double Lambda)
{
	int row, col, i, j;
	double Distance_result, Velocity_result;
	fftw_complex maxComplex;
	maxComplex[0] = maxComplex[1] = -1;
	for (i = 0; i < PulseNumber; i++)
	{
		for (j = 0; j < SampleNumber; j++)
		{
			if (cfar_result[i][j][0] > maxComplex[0] && cfar_result[i][j][1] > maxComplex[1])
			{
				maxComplex[0] = cfar_result[i][j][0];
				maxComplex[1] = cfar_result[i][j][1];
				row = i;
				col = j;
			}
		}
	}
	peaks[loop][0] = maxComplex[0];
	peaks[loop][1] = maxComplex[1];

	if (peaks[loop][0] != 0)
	{
		Distance_result = col * 30 / 2.0;
		Velocity_result = row * PRF * Lambda / (2 * PulseNumber);
		printf("Loop %dDistance %lf Velocity %lf\n",  loop, Distance_result, Velocity_result);
	}
}