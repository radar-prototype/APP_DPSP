#include "slc.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "rsplBspHead.h"
// 初始化队列
MSG_Q_ID Slc::queue_handle_1;//内部队列
MSG_Q_ID Slc::queue_receive_1;//接收队列


// 构造函数
Slc::Slc(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Slc::init(){
    // 参数初始化赋值
    pCom_slc.cMainBeamCnt = 4;
	pCom_slc.cSideBeamCnt = 1;
	pCom_slc.num_sRgeCell = 200;
	pCom_slc.num_thread = 2;
	pCom_slc.sCPINum = 32;
	pCom_slc.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "slc";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Slc::queue_handle_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Slc::queue_receive_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Slc::run(int *thread_id){
    // 创建线程
    
    // 默认处理函数循环
    for (int i = 0; i < pCom_slc.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_slc_thread[i], NULL,Slc::fun_handle_1, (void *)&thread_id[i]);
    }
	pthread_create(&tidp_slc, NULL, Slc::fun_receive_thread_1,(void*)&(pCom_slc));
	
}

void Slc::join(){
    // 把该组件的join写进去
    
    for (int i = 0; i < pCom_slc.num_thread; i++)
    {
        pthread_join(tidp_slc_thread[i], NULL);
    }
	pthread_join(tidp_slc, NULL);

}
// 请修改以下函数的实际内容 注意队列命名
void* Slc::fun_handle_1(void* arg){//用户填写函数内的代码
	
}  
void* Slc::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	
}  


