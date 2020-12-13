#include <stdlib.h>
#include "fftw3.h"
#include <iostream>
#define PulseNumber 8
#define SampleNumber 2700
using namespace std;
void writeData(const char* fileName,fftw_complex **complex_array){
    FILE *fp;
    if ((fp = fopen(fileName,"w+"))){
    }
    else{
        printf("can not open file");
    }
    int i,j;
    for ( i = 0; i < PulseNumber; i++)
    {
        for (j = 0;j < 40000;j++){
            char* temp_r = new char[255];
            char* temp_i = new char[255];
            gcvt(complex_array[i][j][0],10,temp_r);
            gcvt(complex_array[i][j][1],10,temp_i);
            fputs(temp_r,fp);
            fputs(" ",fp);
            fputs(temp_i,fp);
            fputs("i    ",fp);
            if ((j+1) % 5 == 0)
            {
                fputs("\n",fp);
            }
            delete(temp_i);
            delete(temp_r);
        }
    }
    
}