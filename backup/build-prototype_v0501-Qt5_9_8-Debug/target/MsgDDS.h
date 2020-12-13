/*
 * RspMsgDDS.h
 *
 *  Created on: 2020年7月6日
 *      Author: root
 */


#ifndef RSP_MSGDDS_H_
#define RSP_MSGDDS_H_

int ddsSubscribe(char* Component_Name,  char* buffer, unsigned int nBytes,  int timeout);
int ddsPublish(char* component_name,  char* buffer,unsigned int nBytes, int timeout, int priority);

#endif /* RSP_MSGDDS_H_ */
