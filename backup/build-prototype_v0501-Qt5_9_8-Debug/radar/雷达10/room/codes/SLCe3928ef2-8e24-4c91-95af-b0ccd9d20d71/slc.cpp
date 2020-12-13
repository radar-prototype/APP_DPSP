#include "slc.h"

// 初始化队列
MSG_Q_ID Slc::queue_handle_1;//内部队列
MSG_Q_ID Slc::queue_receive_1;//接收队列
pthread_mutex_t Slc::g_slc_thread;

// 构造函数
Slc::Slc(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Slc::init(){
	// 用户自己写
	pthread_mutex_init(&Slc::g_slc_thread, NULL);
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
    msgQCreate(&(Slc::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Slc::queue_receive_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
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

// 请修改以下函数的实际内容 注意队列命名
void* Slc::fun_handle_1(void* arg){//用户填写函数内的代码
	rspl_thread_param slc;
	pthread_t tidp;
	int thread_id = *((int*)arg);
	RSPL_SLC_ENV* pEnv;
	unsigned int len;

	struct timeval tv_b,tv_e;
	unsigned int time_used;

	while(1)
	{
		msgQReceive(&Slc::queue_receive_1, (char*)&slc, sizeof(rspl_thread_param), 0);

		gettimeofday(&tv_b,0);
		pEnv = (RSPL_SLC_ENV*)slc.pEnv;
		len = rspl_sidelobe_canceller(pEnv,
										rspl_comm_get_romate_datbuf(slc.pIn),
										rspl_comm_get_romate_datbuf(slc.pOut),
										thread_id);
		rspl_comm_clear_romate_flag(slc.pIn);

		pthread_mutex_lock(&g_slc_thread);
		rspl_comm_count_romate_fwd(slc.pOut, 0);
		pthread_mutex_unlock(&g_slc_thread);

		gettimeofday(&tv_e,0);
		time_used = rspl_comm_getime(tv_b, tv_e);
		LOG_INFO("slc component thread use time %d us!\n", time_used);

//		LOG_INFO("slc thread_id = %d\n", thread_id);
	}

	return NULL;
}  
void* Slc::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	RSPL_SLC_ICOMP* pComp = (RSPL_SLC_ICOMP*)arg;
	RSPL_SLC_ENV* pEnv;
	RSPL_LINKING_ICOMP* pOut, *pRecv;
	rspl_thread_param dat, slc, csa;
	unsigned int len;
	unsigned int nFrameCnt;

	if(pComp)
	{
		pEnv = rspl_sidelobe_canceller_init(pComp, &pOut);
		if(pEnv)
		{
			while(1)
			{
				msgQReceive(&Dpc::queue_handle_1, (char*)&dat, sizeof(rspl_thread_param), 0);

				len = rspl_sidelobe_canceller_entry(rspl_comm_get_romate_datbuf(dat.pIn), pOut, &pRecv);			//
				if(len)
				{
					nFrameCnt = rspl_comm_get_cnt(pOut->prev, 0);
					if(nFrameCnt)
					{
						 rspl_comm_set_cnt(pOut->prev,0, 0);
						 csa.pEnv = NULL;
						 csa.pIn = pOut->prev->pbuf;
						 csa.pOut = NULL;
                         //printf("slc-----------------------------------------len===========%d\n",len); //262432
						 msgQSend(&Slc::queue_handle_1, (char*)&csa, sizeof(rspl_thread_param), 0, 1);
                         //msgQSendNext(NULL, csa.pIn, len, 0, 1,&param_dpc);  
						 ///////////////////debug/////////////////////////////////////////////
						 //rspl_comm_clear_flag(pOut->prev);
						 //////////////////////////////////////////////////////////////////////////
					}
					if((pOut != pRecv) && (pOut == pRecv->prev))
						pOut = pOut->next;
				}
				slc.pEnv = (char*)pEnv;
				slc.pIn = dat.pIn;
				slc.pOut = pRecv->pbuf;
				msgQSend(&Slc::queue_receive_1, (char*)&slc, sizeof(rspl_thread_param), 0, 1);
			}
			rspl_sidelobe_canceller_uninit(pEnv, pComp, pOut);
		}
	}
}  

void Slc::join(){
    for (int i = 0; i < pCom_slc.num_thread; i++)
    {
        pthread_join(tidp_slc_thread[i], NULL);
    }
	pthread_join(tidp_slc, NULL);
}


