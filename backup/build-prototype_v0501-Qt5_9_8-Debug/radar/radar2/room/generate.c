#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "RspMsgDDS.h"

void *generateData(void *arg)
{   
    sleep(1);
    while (1)
    {
        ddsPublish("sim", "888888888");
    }
}

void *ddsData(char *arg)
{
    while (1)
    {
        printf("ddsname%s",arg);
        char *p = (char *)malloc(15 * sizeof(char));
        strcpy(p, arg);
        char *buffer = (char *)malloc(15 * sizeof(char));
        ddsSubscribe(p, buffer, 15, 0);
        printf("ddsname %s ddsdata %s\n", p, buffer);
        free(p);
    }
}

int main(int argc, char const *argv[])
{
        int i, thread_id[2];
        pthread_t tidp_generate;
        //���������������  
        //2Ϊ�������Ҳ���̸߳���
        pthread_t tidp_dds[2];

        //�������
        char *componet[] = {"sim","test2"};
        int thread_num = 2;

        //������������߳�
        pthread_create(&tidp_generate, NULL, generateData, NULL);

        for (i = 0; i < thread_num; i++)
        {
            thread_id[i] = i;
            //������������߳�
            pthread_create(&tidp_dds[i], NULL, ddsData, componet[i]);
            //pthread_create(&tidp_dds[i], NULL, ddsData, componet[1]);
        }

        pthread_join(tidp_generate, NULL);

        for (i = 0; i < thread_num; i++)
        {
            pthread_join(tidp_dds[i], NULL);
        }

    return 0;
}



