#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void run(char *fileName, char * outName){
    char *a = "g++ ";
    char *b = " -fopenmp -lfftw3 -lm -o ";
    char *c = " && ";
    char *command = (char*)malloc(strlen(a)+strlen(b)+strlen(fileName)+strlen(outName)*2);
    strcat(command,a);
    strcat(command,fileName);
    strcat(command,b);
    strcat(command,outName);
    strcat(command,c);
    strcat(command,outName);
    printf("%s",command);
    system(command);
}

int main(){
    run("radar_test.cpp", "./radar_test.out");
}