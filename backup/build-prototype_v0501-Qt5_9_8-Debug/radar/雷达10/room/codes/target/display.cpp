#include "show_wave.h"
// 初始化队列
MSG_Q_ID Show_wave::queue_receive_1;//接收队列


// 构造函数
Show_wave::Show_wave(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Show_wave::init(){
    // 参数初始化赋值
    
    // 集群相关初始化赋值
    component_name = "display";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分
    msgQCreate(&(Show_wave::queue_receive_1), 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Show_wave::run(int *thread_id){
    // 创建线程
    	pthread_create(&tidp, NULL, Show_wave::fun_receive_thread_1,(void*)&(pCom));
	
		pthread_join(tidp, NULL);

}

// 请修改以下函数的实际内容 注意队列命名
void* Show_wave::fun_receive_thread_1(void* args){//用户填写函数内的代码
	
}  


