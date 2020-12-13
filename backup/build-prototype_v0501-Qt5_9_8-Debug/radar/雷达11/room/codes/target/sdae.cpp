#include "sdae.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "rsplBspHead.h"
// 初始化队列
MSG_Q_ID Sdae::queue_receive_1;//接收队列
MSG_Q_ID Sdae::queue_receive_2;//接收队列
MSG_Q_ID Sdae::queue_handle_1;//内部队列


// 构造函数
Sdae::Sdae(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Sdae::init(){
    // 参数初始化赋值
    pCom_sdae.cMainBeamCnt = 4;
	pCom_sdae.num_thread = 4;
	pCom_sdae.sCPINum = 32;
	pCom_sdae.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "sdae";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Sdae::queue_receive_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Sdae::queue_receive_2), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Sdae::queue_handle_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Sdae::run(int *thread_id){
    // 创建线程
    	pthread_create(&tidp_sdae, NULL, Sdae::fun_receive_thread_1,(void*)&(pCom_sdae));
		pthread_create(&tidp_sdae, NULL, Sdae::fun_receive_thread_2,(void*)&(pCom_sdae));
	
    // 默认处理函数循环
    for (int i = 0; i < pCom_sdae.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_sdae_thread[i], NULL,Sdae::fun_handle_1, (void *)&thread_id[i]);
    }

}

void Sdae::join(){
    // 把该组件的join写进去
    pthread_join(tidp_sdae, NULL);
	pthread_join(tidp_sdae, NULL);

    for (int i = 0; i < pCom_sdae.num_thread; i++)
    {
        pthread_join(tidp_sdae_thread[i], NULL);
    }

}
// 请修改以下函数的实际内容 注意队列命名
void* Sdae::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	
}  
void* Sdae::fun_receive_thread_2(void* arg){//用户填写函数内的代码
	
}  
void* Sdae::fun_handle_1(void* arg){//用户填写函数内的代码
	
}  


