#ifndef DPC_H
#define DPC_H
#include "Common.h"
#include "MsgQueue.h"
#include "sim.h"
class Dpc : public Common
{
public:
    // 组件属性
    RSPL_DPC_ICOMP pCom_dpc;
    // 组件接收线程
    pthread_t tidp_dpc;
    // 组件处理线程
    pthread_t tidp_dpc_thread[8];
    // 组件的名字
    string component_name;
    // 组件的ip
    string component_ip;

    //参数thread_id:检测线程顺序
	static MSG_Q_ID queue_handle_1;//组件结果暂存队列 --处理线程负责将数据取出处理后发送到这个队列 
	static MSG_Q_ID queue_receive_1;//组件接收队列 --接收线程负责将数据取出放到内部队列  
	

    // 构造
    Dpc();

    void init();

    void run(int *thread_id);

    void uninit();

    void join();

    static void* fun_handle_1(void* arg);//用户自己配置 
	static void* fun_receive_thread_1(void* arg);//用户自己配置  
	
};
#endif /* DPC_H */

