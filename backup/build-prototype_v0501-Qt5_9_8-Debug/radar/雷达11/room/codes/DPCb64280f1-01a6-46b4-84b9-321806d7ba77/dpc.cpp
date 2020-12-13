#include "dpc.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "rsplBspHead.h"
// 初始化队列
MSG_Q_ID Dpc::queue_handle_1;//内部队列
MSG_Q_ID Dpc::queue_receive_1;//接收队列


// 构造函数
Dpc::Dpc(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Dpc::init(){
    // 参数初始化赋值
    pCom_dpc.cMainBeamCnt = 4;
	pCom_dpc.cSideBeamCnt = 1;
	pCom_dpc.filename = files/FLM_25.6_5_2048.txt;files/FLM_25.6_5_1024.txt;files/FLM_25.6_10_2048.txt;files/FLM_25.6_5_1024.txt;
	pCom_dpc.num_filter = 4;
	pCom_dpc.num_thread = 2;
	pCom_dpc.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "dpc";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Dpc::queue_handle_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Dpc::queue_receive_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Dpc::run(int *thread_id){
    // 创建线程
    
    // 默认处理函数循环
    for (int i = 0; i < pCom_dpc.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_dpc_thread[i], NULL,Dpc::fun_handle_1, (void *)&thread_id[i]);
    }
	pthread_create(&tidp_dpc, NULL, Dpc::fun_receive_thread_1,(void*)&(pCom_dpc));
	
}

void Dpc::join(){
    // 把该组件的join写进去
    
    for (int i = 0; i < pCom_dpc.num_thread; i++)
    {
        pthread_join(tidp_dpc_thread[i], NULL);
    }
	pthread_join(tidp_dpc, NULL);

}
// 请修改以下函数的实际内容 注意队列命名
void* Dpc::fun_handle_1(void* arg){//用户填写函数内的代码
	
}  
void* Dpc::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	
}  


