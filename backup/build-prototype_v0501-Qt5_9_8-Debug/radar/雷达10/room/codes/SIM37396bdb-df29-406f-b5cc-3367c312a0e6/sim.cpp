#include "sim.h"

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
    msgQCreate(&(Sim::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	    // 这块注意修改log文件的位置
    rspl_initialize(RLL_INFO, RLT_CONSOLE, "files/rspl.log");
}

// 运行函数
void Sim::run(int *thread_id){
    // 创建线程
    // 默认处理函数循环
    pthread_create(&tidp_sim, NULL,Sim::fun_handle_1, (void *)&(pCom_sim));
	
}

// 请修改以下函数的实际内容 注意队列命名
void* Sim::fun_handle_1(void* arg){//用户填写函数内的代码
	RSPL_SIM_ICOMP* pComp = (RSPL_SIM_ICOMP*)arg;
	RSPL_SIM_ENV* pEnv;
	RSPL_LINKING_ICOMP* pOut, *pLink;
	char* pbuf;
	int nFrame = 0;
	rspl_thread_param  tpsim;
	unsigned int nOverflow = 0;
    int n =100;
	struct timeval tv_b,tv_e;
	unsigned int time_used;
    int i=0,Num =0;
	if(pComp)
	{
		pEnv = rspl_simulate_target_init(pComp, &pOut);
		if(pEnv)
		{
			usleep(1000000);
			while(1)
			{
              // i++;
				pLink = rspl_comm_buff_overflow(pOut);
				if(pLink)
				{
                    Num++;
					gettimeofday(&tv_b,0);
					rspl_comm_set_flag(pLink, RBS_OCCUPY);
					pbuf = rspl_comm_get_datbuf(pLink);
					rspl_simulate_target_entry(pEnv, &nFrame, pbuf);
					gettimeofday(&tv_e,0);
					time_used = rspl_comm_getime(tv_b, tv_e);
					//LOG_INFO("simulate component use time %d us!\n", time_used);
                    printf("sim finish  NUM:%d\n", Num);
              
					tpsim.pEnv = NULL;
					tpsim.pIn = pLink->pbuf;
					tpsim.pOut = NULL;
					msgQSend(&Sim::queue_handle_1, (char*)&tpsim, sizeof(rspl_thread_param), 0, 1);

					pOut = pLink->next;
				}
				else
				{
					LOG_WARNING("simulate component buffer is overflow!\n");
				}
                //sleep(1);
				usleep(500000);
			}
			//rspl_simulate_target_uninit(pEnv, pComp, pOut);
		}
	}
}  

void Sim::join(){

  pthread_join(tidp_sim, NULL);


}
