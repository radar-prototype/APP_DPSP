/*
 * RspMsgQueue.c
 *
 *  Created on: 2020年7月6日
 *      Author: root
 */

#include "MsgQueue.h"


void msgQCreate(MSG_Q_ID* msg, int maxMsgs, int maxMsgLength, int options)
{
	int i=0;
	MSG_Q_LIST *pNode=NULL;

	msg->maxMsgs = maxMsgs;
	msg->maxMsgLength = maxMsgLength;
	msg->options = options;
	msg->MsgCount = 0;
//	sem_init(&msg->semMsg, 0, maxMsgLength);	//
//	for(i=0; i<maxMsgLength; i++)
//	{
//		sem_wait(&msg->semMsg);
//	}

	pthread_mutex_init(&msg->msg_mutex, NULL);
	pthread_cond_init(&msg->msg_cond, NULL);
	if(maxMsgs)
	{
        msg->flag = 0; //队列未满
		msg->msgQ = (MSG_Q_LIST*)malloc(sizeof(MSG_Q_LIST));
		if(msg->msgQ)
		{
			msg->msgQ->Msgbuf = (char*)malloc(maxMsgLength);
            msg->msgQ->flag = -1; //空数据
			pNode = msg->msgQ;
		}
		for(i=1; i<maxMsgs; i++)
		{
			pNode->next = (MSG_Q_LIST*)malloc(sizeof(MSG_Q_LIST));
			if(pNode->next)
			{
				pNode->next->Msgbuf = (char*)malloc(maxMsgLength);
                pNode->next->flag = -1; //空数据
				pNode = pNode->next;
			}
		}
		pNode->next = msg->msgQ;
	}
	msg->read = msg->msgQ;
	msg->write = msg->msgQ;
}

rspl_sta msgQSend(MSG_Q_ID* msgQId,  char* buffer, unsigned int nBytes,  int timeout, int priority)
{
/*	if(msgQId->write)
	{
		pthread_mutex_lock(&msgQId->msg_mutex);
		memcpy(msgQId->write->Msgbuf, buffer, nBytes);
		msgQId->write = msgQId->write->next;
		pthread_mutex_unlock(&msgQId->msg_mutex);
		sem_post(&msgQId->semMsg);
	}*/

		pthread_mutex_lock(&msgQId->msg_mutex);
		//队列满
		if (msgQId->flag == 1)
		{
			pthread_mutex_unlock(&msgQId->msg_mutex);
			return 1;
		}
		memcpy(msgQId->write->Msgbuf, buffer, nBytes);
		msgQId->write->flag = 0;
		msgQId->write = msgQId->write->next;
        msgQId->MsgCount ++;
	    pthread_cond_broadcast(&msgQId->msg_cond);
		if (msgQId->write->flag == 0)
		{
			msgQId->flag = 1;
		}
		pthread_mutex_unlock(&msgQId->msg_mutex);

	return 0;
}

rspl_sta msgQReceive(MSG_Q_ID* msgQId, char* buffer, unsigned int maxNBytes,  int timeout)
{
/*	if(msgQId->read)
	{
		sem_wait(&msgQId->semMsg);
		pthread_mutex_lock(&msgQId->msg_mutex);
		memcpy(buffer, msgQId->read->Msgbuf, maxNBytes);
		msgQId->read = msgQId->read->next;
		pthread_mutex_unlock(&msgQId->msg_mutex);
	}*/

	    pthread_mutex_lock(&msgQId->msg_mutex);
		if (msgQId->read->flag == 1)
		{
             while(msgQId->MsgCount <= 0)
	        {
		     pthread_cond_wait(&msgQId->msg_cond, &msgQId->msg_mutex);
	        }
            msgQId->MsgCount --;
			pthread_mutex_unlock(&msgQId->msg_mutex);
			return -1; //读速度大于写速度
		}
        while(msgQId->MsgCount <= 0)
	    {
		pthread_cond_wait(&msgQId->msg_cond, &msgQId->msg_mutex);
	    }
		memcpy(buffer, msgQId->read->Msgbuf, maxNBytes);
		msgQId->flag = 0;
		msgQId->read->flag = 1;
		msgQId->read = msgQId->read->next;
        msgQId->MsgCount --;
		pthread_mutex_unlock(&msgQId->msg_mutex);

	return 0;
}


rspl_sta msgQDelete(MSG_Q_ID msgQId)
{
	MSG_Q_LIST* pNode, *pLink;


//	sem_destroy(&msgQId.semMsg);
	pthread_cond_destroy(&msgQId.msg_cond);
	pthread_mutex_destroy(&msgQId.msg_mutex);

	if(msgQId.msgQ)
	{
		pNode = msgQId.msgQ->next;		//cut the lingking
		msgQId.msgQ->next = NULL;

		while(pNode)
		{
			pLink = pNode->next;
			if(pNode->Msgbuf)
			{
				free(pNode->Msgbuf);
				pNode->Msgbuf = NULL;
			}
			free(pNode);
			pNode = pLink;
		}

		msgQId.read = NULL;
		msgQId.write = NULL;
	}

	return 0;
}

rspl_sta msgQSendNext(MSG_Q_ID *msgQId, char *buffer, unsigned int nBytes, int timeout, int priority, Component_Param *param)
{
    //保存当前组件
    Component_Param* now=param;

	//第一个生成数据的节点在其他节点空跑
	//获取当期主机IP
	char ip[64];
	memset(ip, 0, sizeof(ip));
	get_local_ip(ip);
	if (!strcmp("127.0.0.1", now->component_ip) == 0)//计算的最后一个节点
	{
		return 0;
	}

    if(now->next == NULL){
       printf("11111111111111111111111111");
       ddsPublish(now->component_name, buffer,nBytes,timeout,priority); 
    }
    //获取下一组件链表
    param=param->next;
   if (param){
	if (now->component_ip == param->component_ip)
	{
		msgQSend(msgQId, buffer, nBytes, timeout, priority);
         return 2;
	}
	else
	{
		 //dds 消息发布
		ddsPublish(now->component_name, buffer,nBytes,timeout,priority); 
        return 3;
	}
 }

	return 0;
}

rspl_sta msgQReceivePre(MSG_Q_ID *msgQId, char *buffer, unsigned int maxNBytes, int timeout, Component_Param *param)
{
	//保证当前的组件与本机一致  防止数据错乱
	//获取当前主机IP
	char ip[64];
	memset(ip, 0, sizeof(ip));
	get_local_ip(ip);
	if (!strcmp("127.0.0.1", param->component_ip) == 0)
	{
		return 0;
	}
	if (param->pre->component_ip == param->component_ip)
	{
		msgQReceive(msgQId, buffer, maxNBytes, timeout);
        return 2;
	}
	else
	{
		ddsSubscribe(param->pre->component_name, buffer, maxNBytes, timeout); //dds
        return 3;
	}
}

int get_local_ip(char *ip)
{
	int fd, intrface, retn = 0;
	struct ifreq buf[INET_ADDRSTRLEN];
	struct ifconf ifc;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof(buf);
		// caddr_t,linux内核源码里定义的：typedef void *caddr_t；
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			if (!(ioctl(fd, SIOCGIFADDR, (char *)&buf[1])))
			{
				char *ip_local = (inet_ntoa(((struct sockaddr_in *)(&buf[1].ifr_addr))->sin_addr));
				strcpy(ip, ip_local);
			}
		}
		close(fd);
		return 0;
	}


}
