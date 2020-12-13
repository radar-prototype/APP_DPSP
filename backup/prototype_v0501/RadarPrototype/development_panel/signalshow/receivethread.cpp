#include "receivethread.h"
#include <QDebug>
extern "C" {
#include "RspMsgDDS.h"
}
void ReceiveThread::run()
{
    int i = 0;
    while(1)
    {
        //int length = 82336*2; //暂时记录数组的长度
        //int length = 262432; //slc
        int length = 131360; //mtd 显示点数2040
        //sdae后的数据，长度2048/4096，去除２５６长度的数据
        char *buffer = (char *)malloc(length);//承接传过来的数据
        if(i > 10000000)
        {
            i = 0;
        }
        if(buffer == nullptr)
        {
            qDebug() << "内存分配失败，需要重新分配内存";
            exit(1);
        }

        // 在本地调试的时候需要注释，在银河麒麟上运行时打开
#if 0
        else
        {
            int data_len = ddsSubscribe("dpc",  buffer, 0, 0);
            qDebug() << "数据长度是" << data_len;
            if(data_len <= 0)
            {
                qDebug() << "接收数据长度为0，请检查是否已发送数据";
            }
            else if(data_len > length)//接受数据的实际大小如果大于预定的内存大小，需要重新分配一块新的内存
            {
//                 qDebug() << "原先分配的内存不足，需要重新分配内存";
                free(buffer);
                buffer = NULL;
                buffer = (char *)calloc(data_len, sizeof(char));
                if(buffer == NULL)
                {
                    qDebug() << "扩容后的新内存分配失败，需要重新分配内存";
                    exit(1);
                }
                ddsSubscribe("dpc", buffer, 0, 0);//使用扩容后的内存重新接受数据
//                 this->receiveData(buffer, data_len);
//                 free(buffer);
//                 buffer = NULL;
                // copy
                char *backupdata = NULL;
                backupdata = (char*)malloc(data_len * sizeof(char));
//                 qDebug() << "*****buffer是**********"<<  (void*)&backupdata;
                memcpy(backupdata, buffer, data_len);
//                 qDebug() << "&&&&&&buffer是&&&&&&&&"<<  (void*)&backupdata;
                if(i % receivedata_interval == 0)
                    emit sendAddressAndLen(backupdata, data_len);

            }
            else if(data_len <= length)//原先分配的内存足够使用
            {
                qDebug() << "内存空间足够";

                // copy
                char *backupdata = NULL;
                backupdata = (char*)malloc(data_len * sizeof(char));
//                 qDebug() << "*****buffer是**********"<<  (void*)&backupdata;
                memcpy(backupdata, buffer, data_len);
//                 qDebug() << "&&&&&&buffer是&&&&&&&&"<<  (void*)&backupdata;
                if(i % receivedata_interval == 0)
                {
//                    qDebug() << "&&&&&&buffer是&&&&&&&&"<<  (void*)&buffer;
                    emit sendAddressAndLen(backupdata, data_len);
                }
            }
        }
#endif
        i++;
    }
    exec();
}

ReceiveThread::ReceiveThread()
{

}

int ReceiveThread::getReceivedata_interval() const
{
    return receivedata_interval;
}

void ReceiveThread::setReceivedata_interval(int value)
{
    receivedata_interval = value;
}
