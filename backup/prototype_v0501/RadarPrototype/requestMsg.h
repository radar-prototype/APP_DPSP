#ifndef REQUEST_MSG_H_
#define REQUEST_MSG_H_

int getStatus(char* buffer[]);
int transport();
int getSysInfo(char* buffer[]);
int isAttachable(char* addr);

#endif /* REQUEST_MSG_H_ */
