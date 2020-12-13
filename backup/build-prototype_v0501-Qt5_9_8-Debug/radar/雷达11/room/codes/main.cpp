#include "Common.h"
#include "sim.h"
#include "dpc.h"
#include "slc.h"
#include "mtd.h"
#include "cfar.h"
#include "sdae.h"
int main()
{
    // 总体参数配置 thread_id:检测线程顺序
    int i, thread_id[10];
    for(i=0; i<10; i++)  thread_id[i] = i;

    Sim sim;
    sim.init();
    sim.run(thread_id);

    Dpc dpc;
    dpc.init();
    dpc.run(thread_id);

    Slc slc;
    slc.init();
    slc.run(thread_id);

    Mtd mtd;
    mtd.init();
    mtd.run(thread_id);

    Cfar cfar;
    cfar.init();
    cfar.run(thread_id);

    Sdae sdae;
    sdae.init();
    sdae.run(thread_id);

	sim.join();
	dpc.join();
	slc.join();
	mtd.join();
	cfar.join();
	sdae.join();

    return 0;
}

