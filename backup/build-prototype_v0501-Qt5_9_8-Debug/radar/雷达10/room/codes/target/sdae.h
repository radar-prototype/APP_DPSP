#ifndef SDAE_H
#define SDAE_H
#include "Common.h"
#include "MsgQueue.h"
#include "cfar.h"
class Sdae : public Common
{
public:
    // 组件属性
    RSPL_SDAE_ICOMP pCom_sdae;
    // 组件接收线程
    pthread_t tidp_sdae;
    // 组件处理线程
    pthread_t tidp_sdae_thread[8];

	// 用户自己填写，比较特殊，第二个处理线程
	pthread_t tidp_sdae_port2;

	// 用户新增全局变量
	static RSPL_LINKING_ICOMP* m_sdae_pLink;

	// 用户新增的通信中间件绘图
	static Component_Param param_sdae;

    // 组件的名字
    string component_name;
    // 组件的ip
    string component_ip;

    static MSG_Q_ID queue_receive_1;//组件接收队列 --接收线程负责将数据取出放到内部队列  
	static MSG_Q_ID queue_receive_2;//组件接收队列 --接收线程负责将数据取出放到内部队列  
	//参数thread_id:检测线程顺序
	static MSG_Q_ID queue_handle_1;//组件结果暂存队列 --处理线程负责将数据取出处理后发送到这个队列 
	

    // 构造
    Sdae();

    void init();

    void run(int *thread_id);

    void uninit();

    void join();

    static void* fun_receive_thread_1(void* arg);//用户自己配置  
	static void* fun_receive_thread_2(void* arg);//用户自己配置  
	static void* fun_handle_1(void* arg);//用户自己配置 
	
};
#endif /* SDAE_H */

