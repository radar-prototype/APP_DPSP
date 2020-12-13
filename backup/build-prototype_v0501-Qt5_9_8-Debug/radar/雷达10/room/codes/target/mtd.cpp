#include "mtd.h"

MSG_Q_ID Mtd::queue_receive_1;//接收队列
MSG_Q_ID Mtd::queue_handle_1;//内部队列
MSG_Q_ID Mtd::queue_handle_2;//内部队列
pthread_mutex_t Mtd::g_mtd_thread;

// 构造函数
Mtd::Mtd(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Mtd::init(){
	// 用户自己写
	pthread_mutex_init(&Mtd::g_mtd_thread, NULL);
    // 参数初始化赋值
    pCom_mtd.cMainBeamCnt = 4;
	pCom_mtd.num_thread = 4;
	pCom_mtd.sCPINum = 32;
	pCom_mtd.sMaxRgeCell = 4096;
	
    // 集群相关初始化赋值
    component_name = "mtd";
	component_ip = "127.0.0.1";
	
    // 队列初始化后半部分，注意修改队列大小
    msgQCreate(&(Mtd::queue_receive_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Mtd::queue_handle_1), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&(Mtd::queue_handle_2), 500, sizeof(rspl_thread_param), MSG_Q_FIFO);
	
}

// 运行函数
void Mtd::run(int *thread_id){
    // 创建线程
    pthread_create(&tidp_mtd, NULL, Mtd::fun_receive_thread_1,(void*)&(pCom_mtd));
	
    // 默认处理函数循环
    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_mtd_thread[i], NULL,Mtd::fun_handle_1, (void *)&thread_id[i]);
    }

}

// 请修改以下函数的实际内容 注意队列命名
void* Mtd::fun_receive_thread_1(void* arg){//用户填写函数内的代码
	RSPL_MTD_ICOMP* pComp = (RSPL_MTD_ICOMP*)arg;
	RSPL_MTD_ENV* pEnv;
	RSPL_LINKING_ICOMP* pAmgOut, *pLink, *pIQout, *pIQLink;
	rspl_thread_param slc, mtd;
	unsigned int len;
	unsigned int aLen[RSPL_MAX_BEAM];
	unsigned int i, mainlobe;

	if(pComp)
	{
		pEnv = rspl_target_detection_init(pComp, &pAmgOut, &pIQout);
		if(pEnv)
		{
			while(1)
			{
				msgQReceive(&Slc::queue_handle_1, (char*)&slc, sizeof(rspl_thread_param), 0);

				mainlobe = rspl_target_detection_entry(rspl_comm_get_romate_datbuf(slc.pIn), aLen);			//
				rspl_comm_set_romate_flag(slc.pIn, RBS_BUSY);	//设置发送数据缓冲区
				rspl_comm_set_romate_cnt(slc.pIn, 1, mainlobe);	//记录主瓣个数

				mtd.offset = 0;
				for(i=0; i<mainlobe; i++)
				{
					pLink = rspl_comm_buff_overflow(pAmgOut);
					if(pLink)
					{
						rspl_comm_set_flag(pLink, RBS_OCCUPY);	//设置cfar发送数据缓冲区

						mtd.pEnv = (char*)pEnv;
						mtd.pIn = (char*)slc.pIn;
						mtd.pOut = pLink->pbuf;

						pIQLink = rspl_comm_buff_overflow(pIQout);
						if(pIQLink)
						{
							rspl_comm_set_flag(pIQLink, RBS_OCCUPY);
							mtd.pOut2 = pIQLink->pbuf;
							pIQout = pIQLink->next;	//如果该内存没用，下个数据到来可以继续使用
						}
						else
							LOG_WARNING("target detection buffer2 is overflow\n");

						msgQSend(&Mtd::queue_receive_1, (char*)&mtd, sizeof(rspl_thread_param), 0, 1);

						mtd.offset += aLen[i];
						pAmgOut = pLink->next;
					}
					else
						LOG_WARNING("target detection buffer is overflow\n");
				}
				LOG_DEBUG("send beam data %d\n", mainlobe);
			}
			rspl_target_detection_uninit(pEnv, pComp, pAmgOut, pIQout);
		}
	}
}  
void* Mtd::fun_handle_1(void* arg){//用户填写函数内的代码
	rspl_thread_param dat, mtd, sdae;
	int thread_id = *((int*)arg);
	RSPL_MTD_ENV* pEnv;
	unsigned int len, sdaelen;
	char* pbuf;

	struct timeval tv_b,tv_e;
	unsigned int time_used;

	while(1)
	{
		msgQReceive(&Mtd::queue_receive_1, (char*)&dat, sizeof(rspl_thread_param), 0);

		gettimeofday(&tv_b,0);
		pEnv = (RSPL_MTD_ENV*)dat.pEnv;
		pbuf = rspl_comm_get_romate_datbuf(dat.pIn)+dat.offset;		//对应波束首地址
		len = rspl_target_detection(pEnv,
									pbuf, 	//输入数据缓冲区，同时输出到和差测角
									rspl_comm_get_romate_datbuf(dat.pOut),
									rspl_comm_get_romate_datbuf(dat.pOut2),
									&sdaelen,
									thread_id);
		////////////////////加互斥信号量///////////////////////////////////////////
		pthread_mutex_lock(&g_mtd_thread);
		rspl_comm_count_romate_inv(dat.pIn, 1);	//处理完1个波束进行递减
		if(rspl_comm_get_romate_cnt(dat.pIn, 1) == 0)
		{
			rspl_comm_clear_romate_flag(dat.pIn);		//全部处理完设置清空标志
			LOG_DEBUG("all beam mtd over!\n");
		}
		pthread_mutex_unlock(&g_mtd_thread);

		gettimeofday(&tv_e,0);
		time_used = rspl_comm_getime(tv_b, tv_e);
		LOG_INFO("nca-mti-mtd component thread use time %d us!\n", time_used);

		//////////////////////////debug////////////////////////////////////////////////
//		rspl_comm_clear_romate_flag(dat.pOut);		//clear out buffer
//		rspl_comm_clear_romate_flag(dat.pOut2);
		////////////////////////////////////////////////////////////////////////////////////////

		mtd.pEnv = NULL;
		mtd.pIn = (char*)dat.pOut;
		mtd.pOut = NULL;
		msgQSend(&Mtd::queue_handle_1, (char*)&mtd, sizeof(rspl_thread_param), 0, 1);

        //msgQSendNext(NULL, mtd.pIn, len, 0, 1,&param_dpc);  
		if(sdaelen)		//发和差测角组件
		{
			rspl_comm_set_romate_flag(dat.pOut2, RBS_OCCUPY);	//设置cfar发送数据缓冲区
			sdae.pEnv = NULL;
			sdae.pIn = dat.pOut2;	//发送IQ数据用于和差测角
			sdae.pOut = NULL;
			msgQSend(&Mtd::queue_handle_2, (char*)&sdae, sizeof(rspl_thread_param), 0, 1);
            //msgQSendNext(NULL, sdae.pIn, len, 0, 1,&param_dpc);  
		}
		else
			rspl_comm_clear_romate_flag(dat.pOut2);	//没有要发送的数据，就清缓冲区发送标志
    }
	return NULL;
}

void Mtd::join(){
    pthread_join(tidp_mtd, NULL);

    for (int i = 0; i < pCom_mtd.num_thread; i++)
    {
        pthread_join(tidp_mtd_thread[i], NULL);
    }
  
}

