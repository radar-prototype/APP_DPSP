
#include <stdlib.h>
#include <vsip.h>
#include <string.h>
#include "constant.h"
void readData(const char *fileName,vsip_cmview_f *data,int index,int sign){
    FILE *fp;
    if(fp = fopen(fileName,"r")){
    }
    else{
        printf("can not open file!");
        return;
    }
    int i = 0,row = 0,col = 0;
    double real_part = 0.0,imaginary_part = 0.0;
    char *real_part_temp = (char*)malloc(sizeof(char)*100);
    char *imaginary_part_temp = (char*)malloc(sizeof(char)*100);
    while (i < 16*600+index)
    {
        if (feof(fp)){
            break;
        }
        fscanf(fp,"%s",real_part_temp);
        fscanf(fp,"%s",imaginary_part_temp);
        real_part = strtod(real_part_temp,NULL);
        imaginary_part = strtod(imaginary_part_temp,NULL);
        if (i >= index){
            vsip_cscalar_f temp;
            temp.r = real_part;
            temp.i = imaginary_part;
            vsip_cmput_f(data,row,col,temp);
            col ++;
            if(col > 0 && (col) % 600 == 0){
                row++;
                col = 0;
            }
        }
        if (sign == 1){
            fscanf(fp,"%s",real_part_temp);
        }    
        i++;
    }
}
/**
 * data 是16*600个复数，所以需要16*600*2个数据来存储，即16*1200
*/
void read_data_as_double(const char *fileName,double *data,int index,int sign){
    FILE *fp;
    if(fp = fopen(fileName,"r")){
    }
    else{
        printf("can not open file!");
        return;
    }
    int i = 0,row = 0,col = 0;
    double real_part = 0.0,imaginary_part = 0.0;
    char *real_part_temp = (char*)malloc(sizeof(char)*100);
    char *imaginary_part_temp = (char*)malloc(sizeof(char)*100);
    while (i < 16*600+index)
    {
        if (feof(fp)){
            break;
        }
        fscanf(fp,"%s",real_part_temp);
        fscanf(fp,"%s",imaginary_part_temp);
        real_part = strtod(real_part_temp,NULL);
        imaginary_part = strtod(imaginary_part_temp,NULL);
        if (i >= index){
            data[row * SampleNumber * 2 + col] = real_part;//real part
            data[row * SampleNumber * 2 + col + 1] = imaginary_part;// imaginary part
            col += 2;
            if(col > 0 && (col) % (2*600) == 0){
                row++;
                col = 0;
            }
        }
        if (sign == 1){
            fscanf(fp,"%s",real_part_temp);
        }    
        i++;
    }
}