#include <vsip.h>
#include <fftw3.h>
# define length 10
void transfer_float2vsip(){
    float *source_data = (float *)malloc(sizeof(float)*length*2);
    int i = 0;
    for(i = 0; i < length * 2; i++){
        *(source_data+i) = i;
    }
    // for(i = 0; i < length; i++){
    //     printf("a[%d] = %.2f + %.2f*I\n",i,source_data[2 * i],source_data[2 * i + 1]);
    // }
    fftwf_complex *a = (fftwf_complex*)malloc(sizeof(fftwf_complex) * length);
    a = source_data;
    printf("==================================================================\n");
    for (i = 0; i < length; i++)
    {
        printf("fftw format: the read part is: %f, the img part is: %f\n",a[i][0],a[i][1]);
    }
    float *data = (float*)malloc(sizeof(float) * length * 2);
    data = a;
    for (i = 0; i < length; i++)
    {
        printf("data[%d] = %.2f + %.2f*I\n",i,data[2 * i],data[2 * i + 1]);
    }
    
    // printf("==================================================================\n");
    // xd_vsip_cblock_f *c_block_a = xd_vsip_cblockbind_f(source_data,NULL,length,XD_VSIP_MEM_CONST);
    // xd_vsip_cvview_f *c_view_a = xd_vsip_cvbind_f(c_block_a,0,1,length);
    // for (i = 0;i < length; i++){
    //     printf("vsip format: the real part is: %f, the img part is: %f\n",xd_vsip_cvget_f(c_view_a,i).r,xd_vsip_cvget_f(c_view_a,i).i);
    // }
    //     for(i = 0; i < length; i++){
    //     printf("a[%d] = %.2f + %.2f*I\n",i,source_data[2 * i],source_data[2 * i + 1]);
    // }
}

void transfer_vsip2float(){
    xd_vsip_cvview_f *c_view = xd_vsip_cvcreate_f(length,XD_VSIP_MEM_CONST);
    int i = 0,j = 0;
    for (i = 0; i < length; i++){
        xd_vsip_cvput_f(c_view,i,xd_vsip_cmplx_f(i,2*i));
    }
    for (i = 0; i < length; i++)
    {
        printf("%f,%f\n",xd_vsip_cvget_f(c_view,i).r,xd_vsip_cvget_f(c_view,i).i);
    }

    float *data = (float*)malloc(sizeof(float) * length * 2);
    for (i = 0; i < length; i++)
    {
        xd_vsip_cvputoffset_f(c_view,i);
        xd_vsip_cscalar_f temp = xd_vsip_cvget_f(c_view,0);
        data[i*2] = temp.r;
        data[i*2+1] = temp.i;
    }

    for (i = 0; i < length; i++)
    {
        printf("the real part is: %f,the imag part is: %f\n",data[i*2],data[i*2+1]);
    }
}


int main(){
    transfer_float2vsip();
}