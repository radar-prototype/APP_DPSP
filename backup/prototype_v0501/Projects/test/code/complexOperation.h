#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "fftw3.h"
void multiplication(fftw_complex* result,fftw_complex *array1,fftw_complex *array2,int length);
void scaleValue(fftw_complex *result,int scale,int length);
void complexMeanValue(fftw_complex **result,int index,int leftIndex,int rightIndex,double *rp,double *ip);
void sliceData(fftw_complex *complex_array,int leftIndex,int rightIndex,fftw_complex *result,int inLeftIndex);
void findMaxRowAndColumn(fftw_complex** cfar_result, fftw_complex* peaks, int loop, double PRF, double Lambda);