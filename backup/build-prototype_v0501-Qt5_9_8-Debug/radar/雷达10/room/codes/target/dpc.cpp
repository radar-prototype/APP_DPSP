#include "dpc.h"

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
	pCom_dpc.num_filter = 4;
	pCom_dpc.num_thread = 2;
	pCom_dpc.sMaxRgeCell = 4096;
	pCom_dpc.filename = (char**)malloc(sizeof(char*)*pCom_dpc.num_filter);
	pCom_dpc.filename[0] = "files/FLM_25.6_5_2048.txt";
	pCom_dpc.filename[1] = "files/FLM_25.6_5_1024.txt";
	pCom_dpc.filename[2] = "files/FLM_25.6_10_2048.txt";
	pCom_dpc.filename[3] = "files/FLM_25.6_10_1024.txt";
	
    // 集群相关初始化赋值
    component_name = "dpc";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Dpc::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Dpc::queue_receive_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Dpc::run(int *thread_id){
    // 创建线程
    pthread_create(&tidp_dpc, NULL, Dpc::fun_receive_thread_1,(void*)&(pCom_dpc));
    // 默认处理函数循环
    for (int i = 0; i < pCom_dpc.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_dpc_thread[i], NULL,Dpc::fun_handle_1, (void *)&thread_id[i]);
    }

}

// 请修改以下函数的实际内容 注意队列命名
void* Dpc::fun_handle_1(void* arg){//用户填写函数内的代码
	rspl_thread_param dpc, slc;
	pthread_t tidp;
	int thread_id = *((int*)arg);
	RSPL_DPC_ENV* pEnv;
	unsigned int len = 82336;
  
	struct timeval tv_b,tv_e;
	unsigned int time_used;
    int Num=0;
	tidp = pthread_self();

	while(1)
	{
		msgQReceive(&Dpc::queue_receive_1, (char*)&dpc, sizeof(rspl_thread_param), 0);
        Num++;
		gettimeofday(&tv_b,0);
        //usleep(2100);
        printf("Dpc thread NUM:%d\n",Num);
		pEnv = (RSPL_DPC_ENV*)dpc.pEnv;
        //memset(rspl_comm_get_romate_datbuf(dpc.pIn),0,82336);
		len = rspl_pulse_compress(pEnv, rspl_comm_get_romate_datbuf(dpc.pIn), thread_id);
		gettimeofday(&tv_e,0);
		time_used = rspl_comm_getime(tv_b, tv_e);
		//LOG_INFO("dpc component thread use time %d us!\n", time_used);
        
		slc.pEnv = NULL;
		slc.pIn = (char*)dpc.pIn;
		slc.pOut = NULL;
        
		//////////////////////////////run//////////////////////////
        //LOG_INFO("dpc thread_id = %d\n", thread_id);
		msgQSend(&Dpc::queue_handle_1, (char*)&slc, sizeof(rspl_thread_param), 0, 1);
        //msgQSendNext(NULL, slc.pIn, len, 0, 1,&param_dpc);  
	}

	return NULL;
}  

void* Dpc::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	RSPL_DPC_ICOMP* pComp = (RSPL_DPC_ICOMP*)arg;
	RSPL_DPC_ENV* pEnv;
	RSPL_LINKING_ICOMP* pOut, *pLink;
	char* pbuf;
	rspl_thread_param dpc, sim;

	struct timeval tv_b,tv_e;
	unsigned int time_used;
    int Num =0;
	if(pComp)
	{
		pEnv = rspl_pulse_compress_init(pComp, &pOut);
		if(pEnv)
		{
			while(1)
			{
				msgQReceive(&Sim::queue_handle_1, (char*)&sim, sizeof(rspl_thread_param), 0);
                Num++;
				pLink = rspl_comm_buff_overflow(pOut);
				if(pLink)
				{
					rspl_comm_set_flag(pLink, RBS_OCCUPY);

					gettimeofday(&tv_b,0);
					pbuf = rspl_comm_get_datbuf(pLink);
					rspl_pulse_compress_dgn_entry(rspl_comm_get_romate_datbuf(sim.pIn), pbuf);
					rspl_comm_clear_romate_flag(sim.pIn);
					gettimeofday(&tv_e,0);
					time_used = rspl_comm_getime(tv_b, tv_e);
					//LOG_INFO("dpc component data copy use time %d us!\n", time_used);
                    printf("Dpc entry NUM:%d\n",Num);
					dpc.pEnv = (char*)pEnv;
					dpc.pIn = pLink->pbuf;
					dpc.pOut = NULL;
                    msgQSend(&Dpc::queue_receive_1, (char*)&dpc, sizeof(rspl_thread_param), 0, 1);

					pOut = pLink->next;
				}
				else
					LOG_WARNING("dpc component buffer is overflow\n");

			}
			rspl_pulse_compress_uninit(pEnv, pComp, pOut);
		}
	}
}  

void Dpc::join(){
  
    pthread_join(tidp_dpc, NULL);

    for (int i = 0; i < pCom_dpc.num_thread; i++)
    {
        pthread_join(tidp_dpc_thread[i], NULL);
    }

}
