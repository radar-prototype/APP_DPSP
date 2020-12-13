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
        int i, thread_id[__THREAD_NUMS__];
        pthread_t tidp_generate;
        //各个组件接收数据  
        //2为组件个数也是线程个数
        pthread_t tidp_dds[__THREAD_NUMS__];

        //组件名字
        char *componet[] = {__COMPS_LIST_BY_ORDER__};
        int thread_num = __THREAD_NUMS__;

        //组件生成数据线程
        pthread_create(&tidp_generate, NULL, generateData, NULL);

        for (i = 0; i < thread_num; i++)
        {
            thread_id[i] = i;
            //组件接收数据线程
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



