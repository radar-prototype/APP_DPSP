#include "RspMsgQueue.h"

MSG_Q_ID gMsgSimQ;
MSG_Q_ID gMsgSimDatQ;
MSG_Q_ID gMsgDpcQ;
MSG_Q_ID gMsgDpcDatQ;
MSG_Q_ID gMsgSlcQ;
MSG_Q_ID gMsgSlcDatQ;

Component_Param param_sim;
Component_Param param_dpc;
Component_Param param_slc;

void *Simulate_Component(void *arg)
{
	while (1)
	{
		char *p = "aaaaaaaaaaaaa";
		msgQSendNext(&gMsgDpcDatQ, p, 15, 0, 1, &param_sim, &param_dpc);
		usleep(200);
	}
}
//dpc数据接收函数
void *PulseCompress_Component(void *arg)
{
	while (1)
	{
		msgQReceivePre(&gMsgSimDatQ, p, 15, 0, &param_sim, &param_dpc);
		msgQSend(&gMsgDpcQ, p, 15, 0, 1);
		free(p);
	}
}

//dpc数据处理线程
void *PulseCompress_thread(void *arg)
{
	while (1)
	{
		char *p = (char *)malloc(15 * sizeof(char));
		msgQReceive(&gMsgDpcQ, p, 15, 0);
		msgQSendNext(&gMsgDpcDatQ, p, 15, 0, 1, &param_dpc, &param_slc);
		free(p);
	}
	return NULL;
}

//slc数据接收函数
void *SidelobeCanceller_Component(void *arg)
{
	while (1)
	{
		msgQReceivePre(&gMsgDpcDatQ, p, 15, 0, &param_dpc, &param_slc);
		msgQSend(&gMsgSlcQ, p, 15, 0, 1);
		free(p);
	}
}

//slc数据处理线程
void *SidelobeCanceller_thread(void *arg)
{
	while (1)
	{
		char *p = (char *)malloc(15 * sizeof(char));
		msgQReceive(&gMsgSlcQ, p, 15, 0);
		msgQSendNext(&gMsgSlcDatQ, p, 15, 0, 1, &param_slc, &param_NULL);
		free(p);
	}
	return NULL;
}


int main(int arg, char *argv[]){
	int i, thread_id[3];
	pthread_t tidp_sim;
	pthread_t tidp_sim_thread[1];
	param_sim.component_name = "sim";
	param_sim.component_ip = "127.0.0.1";
	msgQCreate(&gMsgSimQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&gMsgSimDatQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);

	pthread_t tidp_dpc;
	pthread_t tidp_dpc_thread[1];
	param_dpc.component_name = "dpc";
	param_dpc.component_ip = "127.0.0.1";
	msgQCreate(&gMsgDpcQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&gMsgDpcDatQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);

	pthread_t tidp_slc;
	pthread_t tidp_slc_thread[1];
	param_slc.component_name = "slc";
	param_slc.component_ip = "127.0.0.1";
	msgQCreate(&gMsgSlcQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);
	msgQCreate(&gMsgSlcDatQ, 20, sizeof(rspl_thread_param), MSG_Q_FIFO);

	pthread_create(&tidp_sim, NULL, Simulate_Component, NULL);
	for (i = 0; i < 0; i++){
		thread_id[i] = i;
		pthread_create(&tidp_sim_thread[i], NULL, Simulate_thread, NULL);
	}

	pthread_create(&tidp_dpc, NULL, PulseCompress_Component, NULL);
	for (i = 0; i < 0; i++){
		thread_id[i] = i;
		pthread_create(&tidp_dpc_thread[i], NULL, PulseCompress_thread, NULL);
	}

	pthread_create(&tidp_slc, NULL, SidelobeCanceller_Component, NULL);
	for (i = 0; i < 0; i++){
		thread_id[i] = i;
		pthread_create(&tidp_slc_thread[i], NULL, SidelobeCanceller_thread, NULL);
	}

	pthread_join(tidp_sim, NULL);
	for (i = 0; i < 0; i++){
		pthread_join(tidp_sim_thread[i], NULL);
	}

	pthread_join(tidp_dpc, NULL);
	for (i = 0; i < 0; i++){
		pthread_join(tidp_dpc_thread[i], NULL);
	}

	pthread_join(tidp_slc, NULL);
	for (i = 0; i < 0; i++){
		pthread_join(tidp_slc_thread[i], NULL);
	}

}

