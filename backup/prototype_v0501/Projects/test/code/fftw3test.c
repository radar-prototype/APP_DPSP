#include "constant.h"
#include <complex.h>
#include "fftw3.h"
#include <vsip.h>
#define LEN 10
int main()
{
    vsip_scalar_f v1 = 1.0;
    vsip_cscalar_f v2 = vsip_cmplx_f(1.,3.);
    float complex cf1 = 1+2*I;
    fftwf_complex f1 = 1+3*I;
    vsip_cscalar_f v2;
    // v2 = (vsip_cscalar_f)f1;
    printf("%ld,%ld\n",sizeof(vsip_cscalar_f),sizeof(fftwf_complex));
    printf("%f,%f",crealf(f1),cimagf(f1));
}
/**
 *  gcc -c writer.c -o writer.o -std=gnu9x -Wall -DRELEASE -DLINUX
gcc writer.o -o writer -lvsip -lm

gcc -c reader.c -o reader.o -std=gnu9x -Wall -DRELEASE -DLINUX
gcc reader.o -o reader -lvsip -lm
*/