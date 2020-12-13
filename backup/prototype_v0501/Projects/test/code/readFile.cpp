#include <stdlib.h>
#include "fftw3.h"
#include <iostream>
using namespace std;

int dataLength(const char* filePath){
    int length = 0;
    FILE *fp;
    fp = fopen(filePath,"r");
    char temp[255];
    while (fscanf(fp,"%s",temp) != EOF){
        fscanf(fp,"%s",temp) != EOF;
        length ++;
    }
    return length;
    
}
void readData(const char* filePath,fftw_complex* complex_array,int index,int length,int sign){
    //index is your start point of the file
    //length is the length you want to read
    FILE* fp;
    char *real_part_temp = new char[255];
    char *imaginary_part_temp = new char[255];
    if ((fp = fopen(filePath,"r"))){
    }
    else{
        printf("can not open file");
    }
    int count = 0;
    //count << index;
    double real_part = 0.0, imaginary_part = 0.0;
    while (count < length){
        fscanf(fp,"%s",real_part_temp);
        fscanf(fp,"%s",imaginary_part_temp);
        
        real_part = strtod(real_part_temp,NULL);
        imaginary_part = strtod(imaginary_part_temp,NULL);
        if (count >= index)
        {
            complex_array[index - count][0] = real_part;
            complex_array[index - count][1] = imaginary_part;
            index ++;
        }
        
        if (sign == 1)
        {
            char *temp = new char[255];
            fscanf(fp,"%s",temp);
        }
        count ++;
        
    }
    delete(real_part_temp);
    delete(imaginary_part_temp);
    fclose(fp);
}/*
int main(){
    fftw_complex* echoArray = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 4096);
    readData("./Echoes.txt",echoArray,0,4096,1);
    int i;
    for (i = 0;i < 4096;i++){
        printf("%f+%fi ",echoArray[i][0],echoArray[i][1]);
    }
}*/
