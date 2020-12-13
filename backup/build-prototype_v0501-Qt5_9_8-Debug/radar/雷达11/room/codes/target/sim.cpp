#include "sim.h"
#include "RspThreadHead.h"
#include "RspMsgQueue.h"
#include "rsplBspHead.h"
// 初始化队列
MSG_Q_ID Sim::queue_handle_1;//内部队列


// 构造函数
Sim::Sim(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Sim::init(){
    // 参数初始化赋值
    pCom_sim.cMainBeamCnt = 4;
	pCom_sim.cSideBeamCnt = 1;
	pCom_sim.num_thread = 1;
	pCom_sim.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "sim";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Sim::queue_handle_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	rspl_initialize(RLL_INFO, RLT_CONSOLE, "files/rspl.log");
	
}

// 运行函数
void Sim::run(int *thread_id){
    // 创建线程
    // 默认处理函数循环
    pthread_create(&tidp_sim, NULL,Sim::fun_handle_1, (void *)&(pCom_sim));

}

void Sim::join(){
    // 把该组件的join写进去
    	pthread_join(tidp_sim, NULL);

}
// 请修改以下函数的实际内容 注意队列命名
void* Sim::fun_handle_1(void* arg){//用户填写函数内的代码
	
}  


