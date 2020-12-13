#include <vsip.h>

struct test
{
    int num;
    vsip_cmview_f *b;
};
typedef struct test t_t;

int main(){
    t_t *t1 = (t_t*)malloc(sizeof(t_t));
    t1->num = 10;
    t1->b = vsip_cmcreate_f(10,10,VSIP_ROW,VSIP_MEM_CONST);
    vsip_cmfill_f(vsip_cmplx_f(1.,1.),t1->b);
    printf("%d,%f\n",t1->num,vsip_cmget_f(t1->b,0,0).r);
}