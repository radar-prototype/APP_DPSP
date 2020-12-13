#include "fftw3.h"
#include <iostream>
using namespace std;

int main(){
    fftw_complex *a = new fftw_complex[1];
    a[0][0] = 1;
    a[0][1] = 1;
    cout << a[0][0] << a[0][1] << endl;
    delete(a);
    return 0;
}