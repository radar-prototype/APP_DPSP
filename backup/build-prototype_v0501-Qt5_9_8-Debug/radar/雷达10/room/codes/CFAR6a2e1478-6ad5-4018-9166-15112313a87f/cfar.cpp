#include "cfar.h"


// 初始化队列
MSG_Q_ID Cfar::queue_handle_1;//内部队列
MSG_Q_ID Cfar::queue_receive_1;//接收队列
pthread_mutex_t Cfar::g_cfar_thread;

// 构造函数
Cfar::Cfar(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Cfar::init(){
	pthread_mutex_init(&Cfar::g_cfar_thread, NULL);
    // 参数初始化赋值
    pCom_cfar.cMainBeamCnt = 4;
	pCom_cfar.num_thread = 3;
	pCom_cfar.sCPINum = 32;
	pCom_cfar.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "cfar";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Cfar::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Cfar::queue_receive_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Cfar::run(int *thread_id){
    // 创建线程
    
    // 默认处理函数循环
    for (int i = 0; i < pCom_cfar.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_cfar_thread[i], NULL,Cfar::fun_handle_1, (void *)&thread_id[i]);
    }
	pthread_create(&tidp_cfar, NULL, Cfar::fun_receive_thread_1,(void*)&(pCom_cfar));

}

// 请修改以下函数的实际内容 注意队列命名
void* Cfar::fun_handle_1(void* arg){//用户填写函数内的代码
	rspl_thread_param cfar;
	int thread_id = *((int*)arg);
	RSPL_CFAR_ENV* pEnv;
	unsigned int len;
	struct timeval tv_b,tv_e;
	unsigned int time_used;

	while(1)
	{
		msgQReceive(&Cfar::queue_receive_1, (char*)&cfar, sizeof(rspl_thread_param), 0);

		gettimeofday(&tv_b,0);
		pEnv = (RSPL_CFAR_ENV*)cfar.pEnv;
		len = rspl_mtd_cfar(pEnv,
							rspl_comm_get_romate_datbuf(cfar.pIn),
							rspl_comm_get_romate_datbuf(cfar.pOut),
							thread_id);
		rspl_comm_clear_romate_flag(cfar.pIn);

		////////////////////加互斥信号量///////////////////////////////////////////
		pthread_mutex_lock(&Cfar::g_cfar_thread);
		rspl_comm_count_romate_fwd(cfar.pOut, 0);	//计数主脉冲处理个数
		pthread_mutex_unlock(&Cfar::g_cfar_thread);
		///////////////////////////////////////////////////////////////////////////
		gettimeofday(&tv_e,0);
		time_used = rspl_comm_getime(tv_b, tv_e);
		LOG_INFO("cfar component thread use time %d us!\n", time_used);
	}
}  
void* Cfar::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	int i;
	RSPL_CFAR_ICOMP* pComp = (RSPL_CFAR_ICOMP*)arg;
	RSPL_CFAR_ENV* pEnv;
	char*pbuf;
	RSPL_LINKING_ICOMP *pOut, *pRecv;
	rspl_thread_param dat, cfar, sdae;
	unsigned int len;

	pEnv = rspl_mtd_cfar_init(pComp, &pOut);
	if(pEnv)
	{
		while(1)
		{
			msgQReceive(&Mtd::queue_handle_1, (char*)&dat, sizeof(rspl_thread_param), 0);

			len = rspl_mtd_cfar_entry(rspl_comm_get_romate_datbuf(dat.pIn), pOut, &pRecv);
			rspl_comm_set_romate_flag(dat.pIn, RBS_BUSY);	//设置发送数据缓冲区

			if(len)	//所有主波束处理完成
			{
				if(rspl_comm_get_cnt(pOut->prev, 0))
				{
					rspl_comm_set_cnt(pOut->prev, 0, 0);

					sdae.pEnv = NULL;
					sdae.pIn = pOut->prev->pbuf;	//发送前一个CPI缓冲区
					sdae.pOut = NULL;
					//发送到下个组件进行和差测角处理
					msgQSend(&Cfar::queue_handle_1, (char*)&sdae, sizeof(rspl_thread_param), 0, 1);

					/////////////////////////////debug///////////////////////////////////
//					rspl_comm_clear_flag(pLink->prev);
					////////////////////////////////////////////////////////////////////////////
				}
				if((pOut != pRecv) && (pOut == pRecv->prev))
					pOut = pOut->next;
			}
			cfar.pEnv = (char*)pEnv;
			cfar.pIn = dat.pIn;
			cfar.pOut = pRecv->pbuf;
			msgQSend(&Cfar::queue_receive_1, (char*)&cfar, sizeof(rspl_thread_param), 0, 1);
		}
		rspl_mtd_cfar_uninit(pEnv, pComp, pOut);
	}
}  

void Cfar::join(){
    for (int i = 0; i < pCom_cfar.num_thread; i++)
    {
        pthread_join(tidp_cfar_thread[i], NULL);
    }
	pthread_join(tidp_cfar, NULL);
  
}
