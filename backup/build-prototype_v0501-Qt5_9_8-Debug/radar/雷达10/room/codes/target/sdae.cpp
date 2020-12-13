#include "sdae.h"

MSG_Q_ID Sdae::queue_receive_1;//接收队列
MSG_Q_ID Sdae::queue_receive_2;//接收队列
MSG_Q_ID Sdae::queue_handle_1;//内部队列

Component_Param Sdae::param_sdae;
RSPL_LINKING_ICOMP* Sdae::m_sdae_pLink;

// 构造函数
Sdae::Sdae(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Sdae::init(){
	// 用户新增通信中间件配置
	Sdae::param_sdae.pre=NULL;
    Sdae::param_sdae.component_name = "dpc";
	Sdae::param_sdae.component_ip = "127.0.0.1";
    Sdae::param_sdae.next=NULL;

    // 参数初始化赋值
    pCom_sdae.cMainBeamCnt = 4;
	pCom_sdae.num_thread = 4;
	pCom_sdae.sCPINum = 32;
	pCom_sdae.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "sdae";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Sdae::queue_receive_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Sdae::queue_receive_2), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Sdae::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Sdae::run(int *thread_id){
    // 创建线程
    pthread_create(&tidp_sdae, NULL, Sdae::fun_receive_thread_1,(void*)&(pCom_sdae));
	pthread_create(&tidp_sdae_port2, NULL, Sdae::fun_receive_thread_2, NULL);
	
    // 默认处理函数循环
    for (int i = 0; i < pCom_sdae.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_sdae_thread[i], NULL,Sdae::fun_handle_1, (void *)&thread_id[i]);
    }
	
}

// 请修改以下函数的实际内容 注意队列命名
void* Sdae::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	int i;
	RSPL_SDAE_ICOMP* pComp = (RSPL_SDAE_ICOMP*)arg;
	RSPL_SDAE_ENV* pEnv;
	char*pbuf;
	RSPL_LINKING_ICOMP *pLink, *pOut, *pIntLink;
	rspl_thread_param dat, sdae;
	unsigned int len;

	pEnv = rspl_angle_estimate_init(pComp, &pOut, &pIntLink);
	Sdae::m_sdae_pLink = pIntLink;

	if(pEnv)
	{
		while(1)
		{
			msgQReceive(&Cfar::queue_handle_1, (char*)&dat, sizeof(rspl_thread_param), 0);
			rspl_comm_set_romate_flag(dat.pIn, RBS_BUSY);	//设置上个组件的发送数据缓冲区

			pLink = rspl_comm_buff_overflow(pOut);
			if(pLink)
			{
				rspl_comm_set_flag(pLink, RBS_OCCUPY);	//设置该组件自身的发送缓冲区

				sdae.pEnv = (char*)pEnv;
				sdae.pIn = (char*)dat.pIn;
				sdae.pOut = pLink->pbuf;
				msgQSend(&Sdae::queue_handle_1, (char*)&sdae, sizeof(rspl_thread_param), 0, 1);
				pOut = pLink->next;
			}
			else
				LOG_WARNING("angle estimation buffer is overflow\n");
		}
		rspl_angle_estimate_uninit(pEnv, pComp, pLink, pIntLink);
	}
}  
void* Sdae::fun_receive_thread_2(void* arg){//用户填写函数内的代码
	RSPL_LINKING_ICOMP *pLink;
	rspl_thread_param dat;
	struct timeval tv_b,tv_e;
	unsigned int time_used;

	while(1)
	{
		msgQReceive(&Mtd::queue_handle_1, (char*)&dat, sizeof(rspl_thread_param), 0);

		pLink = rspl_comm_buff_overflow(m_sdae_pLink);
		if(pLink)
		{
			gettimeofday(&tv_b,0);

			rspl_comm_set_flag(pLink, RBS_OCCUPY);	//设置该组件自身的发送缓冲区
			rspl_angle_estimate_entry2(rspl_comm_get_romate_datbuf(dat.pIn),  rspl_comm_get_datbuf(pLink));	//第二个端口接收的数据
			rspl_comm_clear_romate_flag(dat.pIn);	//释放接收数据缓冲区
			m_sdae_pLink = pLink->next;

			gettimeofday(&tv_e,0);
			time_used = rspl_comm_getime(tv_b, tv_e);
			LOG_INFO("sdae component copy data use time %d us!\n", time_used);

		}
		else
			LOG_WARNING("sdae buffer2 is overflow!\n");
	}
}  
void* Sdae::fun_handle_1(void* arg){//用户填写函数内的代码
	int thread_id = *((int*)arg);
	RSPL_SDAE_ENV* pEnv;
	rspl_thread_param sdae;
	unsigned int len;
	struct timeval tv_b,tv_e;
	unsigned int time_used;

	while(1)
	{
		msgQReceive(&Sdae::queue_handle_1, (char*)&sdae, sizeof(rspl_thread_param), 0);

		gettimeofday(&tv_b,0);
		pEnv = (RSPL_SDAE_ENV*)sdae.pEnv;
		len = rspl_angle_estimate(pEnv, m_sdae_pLink,
							rspl_comm_get_romate_datbuf(sdae.pIn),
							rspl_comm_get_romate_datbuf(sdae.pOut),
							thread_id);
        printf("----------------------len= %d\n",len);
        
        if(len!=256){
                   msgQSendNext(NULL, sdae.pOut, len, 0, 1,&Sdae::param_sdae);
        }        

 

		rspl_comm_clear_romate_flag(sdae.pIn);
		rspl_comm_clear_romate_flag(sdae.pOut);

		gettimeofday(&tv_e,0);
		time_used = rspl_comm_getime(tv_b, tv_e);
		LOG_INFO("sdae component thread use time %d us!\n", time_used);
	}
}  

void Sdae::join(){
    pthread_join(tidp_sdae, NULL);
	pthread_join(tidp_sdae_port2, NULL);

    for (int i = 0; i < pCom_sdae.num_thread; i++)
    {
        pthread_join(tidp_sdae_thread[i], NULL);
    }
}

