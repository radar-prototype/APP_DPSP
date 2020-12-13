#include "generatedata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
extern "C"{
#include "RspMsgDDS.h"
}


GenerateData::GenerateData()
{

}

void* GenerateData::generateData(void *arg)
{
    sleep(1);
    //while (1)
    //{

    // Antrn: NOTE 需要配置链接库
//        ddsPublish("sim", "111111111");
//        ddsPublish("sim", "222222222");
//        ddsPublish("sim", "333333333");
//        ddsPublish("sim", "444444444");
    //}
}

void* GenerateData::ddsData(void *arg)
{
    while (1)
    {
        printf("ddsname%s", (char*)arg);
        char *p = (char *)malloc(15 * sizeof(char));
        strcpy(p, (char*)arg);
        char *buffer = (char *)malloc(15 * sizeof(char));
        // Antrn: NOTE 需要配置链接库
//            ddsSubscribe(p, buffer, 15, 0);
        printf("ddsname %s ddsdata %s\n", p, buffer);
        free(p);
    }
}

void GenerateData::run()
{
    int i, thread_id[10];
    pthread_t tidp_generate;
    //各个组件接收数据  2为组件个数也是线程个数
    pthread_t tidp_dds[2];

    //组件名字
    char *componet[] = {"dpc", "slc"};
    int thread_num = 2;

    //组件生成数据线程
    pthread_create(&tidp_generate, nullptr, generateData, nullptr);

    for (i = 0; i < thread_num; i++)
    {
        thread_id[i] = i;
        //组件接收数据线程
        pthread_create(&tidp_dds[i], nullptr, ddsData, componet[i]);
        //pthread_create(&tidp_dds[i], NULL, ddsData, componet[1]);
    }

    pthread_join(tidp_generate, nullptr);

    for (i = 0; i < thread_num; i++)
    {
        pthread_join(tidp_dds[i], nullptr);
    }

    return;
}
