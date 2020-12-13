#ifndef SHOW_WAVE_H
#define SHOW_WAVE_H
#include "Common.h"
#include "MsgQueue.h"

class Show_wave : public Common
{
public:
    //组件属性
    RSPL_DISPLAY_ICOMP pCom;
    //组件接收线程
    pthread_t tidp;
    //组件处理线程
    pthread_t tidp_thread[12];
    //组件的名字
    string component_name;
    //组件的ip
    string component_ip;

    static MSG_Q_ID queue_receive_1;//组件接收队列 --接收线程负责将数据取出放到内部队列  
	

    //构造
    Show_wave();

    void init();

    void run(int *thread_id);

    void uninit();

    static void*  fun_receive_thread_1(void* args);//用户自己配置  
	
};
#endif /* SHOW_WAVE_H */

