#include "mtd.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "rsplBspHead.h"
// 初始化队列
MSG_Q_ID Mtd::queue_receive_1;//接收队列
MSG_Q_ID Mtd::queue_handle_1;//内部队列
MSG_Q_ID Mtd::queue_handle_2;//内部队列

// 构造函数
Mtd::Mtd(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Mtd::init(){
    // 参数初始化赋值
    pCom_mtd.cMainBeamCnt = 4;
	pCom_mtd.num_thread = 4;
	pCom_mtd.sCPINum = 32;
	pCom_mtd.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "mtd";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Mtd::queue_receive_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Mtd::queue_handle_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Mtd::queue_handle_2), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Mtd::run(int *thread_id){
    // 创建线程
    	pthread_create(&tidp_mtd, NULL, Mtd::fun_receive_thread_1,(void*)&(pCom_mtd));
	
    // 默认处理函数循环
    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_mtd_thread[i], NULL,Mtd::fun_handle_1, (void *)&thread_id[i]);
    }

    // 默认处理函数循环
    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_mtd_thread[i], NULL,Mtd::fun_handle_2, (void *)&thread_id[i]);
    }

}

void Mtd::join(){
    // 把该组件的join写进去
    	pthread_join(tidp_mtd, NULL);

    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        pthread_join(tidp_mtd_thread[i], NULL);
    }

    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        pthread_join(tidp_mtd_thread[i], NULL);
    }

}
// 请修改以下函数的实际内容 注意队列命名
void* Mtd::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	
}  
void* Mtd::fun_handle_1(void* arg){//用户填写函数内的代码
	
}  
void* Mtd::fun_handle_2(void* arg){//用户填写函数内的代码
	
}  


