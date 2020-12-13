/*
 * RspMsgQueue.h
 *
 *  Created on: 2020年7月6日
 *      Author: root
 */



#include "Common.h"



#ifndef RSP_MSGQUEUE_H_
#define RSP_MSGQUEUE_H_
#define MSG_Q_FIFO   0x00  //task wait in fifo order

typedef signed int rspl_sta;// 1 队列满 2同一cpu 3不同cpu 


typedef struct MSG_Q_LIST
{
	char* Msgbuf;		//msg content;
	struct MSG_Q_LIST *next;
	int flag;//-1 0 1     -1 空数据  0 有数据未读 1 已读
}MSG_Q_LIST;


typedef struct MSG_Q_ID
{
	MSG_Q_LIST *msgQ;		//message queue;
	MSG_Q_LIST *read;
	MSG_Q_LIST *write;
	int maxMsgs;			//max number of messages in queue;
	int maxMsgLength;		//max length of message;
	int options;
	int MsgCount;	//signal counter
//	sem_t semMsg;
	pthread_mutex_t msg_mutex;
	pthread_cond_t	 msg_cond;
	int flag;//0 1    0 队列未满 1 队列已满
}MSG_Q_ID;

typedef struct Component_Param
{   
    struct Component_Param* pre;
	char*  component_name;
	char*  component_ip;
    struct Component_Param* next;
}Component_Param;






void msgQCreate(MSG_Q_ID* msg, int maxMsgs, int maxMsgLength, int options);
rspl_sta msgQSend(MSG_Q_ID* msgQId,  char* buffer, unsigned int nBytes,  int timeout, int priority);
rspl_sta msgQReceive(MSG_Q_ID* msgQId,  char* buffer, unsigned int maxNBytes,  int timeout);
rspl_sta msgQDelete(MSG_Q_ID msgQId);
rspl_sta msgQSendNext(MSG_Q_ID* msgQId,  char* buffer, unsigned int nBytes,  int timeout, int priority,Component_Param* param);
rspl_sta msgQReceivePre(MSG_Q_ID* msgQId,  char* buffer, unsigned int maxNBytes,  int timeout,Component_Param* param);
int get_local_ip(char *ip);

#endif /* RSP_MSGQUEUE_H_ */
