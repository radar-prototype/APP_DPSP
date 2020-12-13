#include "worker.h"
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
/*
 * 数据处理线程先将数据全部转化到大数组中,之后将大数组的数据进行处理
 * 获取绘图时的纵坐标写到小数组中
 */
worker::worker(QObject *parent) : QObject(parent)
{

}
//bufferlen是buffer的长度,length是一帧显示点的数量
void worker::do_work(char *buffer, int bufferlen,QString draw_datatype,QString data_type, int length, bool save_data)
{

    //buffer已经加过偏移量,data_type是数据类型,bufferlen是buffer的字节数
    receiveData(buffer,data_type ,bufferlen);


    int label= 0;
    //复数 and 交替 and 虚部
    if(data_type == "fualternate_imagineary")
    {
        label= 1;//交替排列描绘虚部,第一个虚部元素是数组的第二个元素
    }
    //复数 and 并列 and 虚部
    else if(data_type == "fusidebyside_imagineary")
    {
        label= 1024;//并列排列显示虚部

    }

    QVector<double> y1;//记录本次展示数据的小数组

    //y是接受的数据转化成的数组,y1是求模或者只保留实部的绘图用数组
    int num = 0;
    int space = length / 2;//记录复数并列排列求模时的偏移量
    while(num < length)// 根据设定好的当前页面显示的点的数量，设置每次从数据中读取多少数据存到数组中进行显示
    {
        if(draw_datatype == "real")//实数
        {
            y1.append(y[label]);
            label++;//将大数组的所有元素加到当前帧的小数组
        }
        else if(draw_datatype == "fualternate_model")//复数交替排列求模
        {
            double model = sqrt(y[label] * y[label] + y[label+1] * y[label+1]);
            y1.append(model);
            label = label+2;
            //qDebug() << "测试：求模后的数据为" <<label<<"："<<model;
        }
        else if(draw_datatype == "fualternate_real" || data_type == "fualternate_imagineary")//复数交替排列实部或者虚部
        {
            y1.append(y[label]);
            label = label+2;
        }
        else if(draw_datatype == "fusidebyside_model")//复数并列排列求模
        {
            double model = sqrt(y[label] * y[label] + y[label+space] * y[label+space]);
            y1.append(model);
            label++;
        }
        else if(draw_datatype == "fusidebyside_real" ||data_type == "fusidebyside_imagineary")
        {
            y1.append(y[label]);
            label++;
        }
        num++;
    }
    static int test_num = 1;
    qDebug() << "数据处理完成,本轮数据批次:" << test_num++;

    if(save_data)
    {
        QFile file("mydata.txt");
        if(!file.open(QIODevice::ReadWrite))
        {
            //QMessageBox::warning(this,"错误","文件打开失败",QMessageBox::Ok|QMessageBox::No);
            qDebug() << "文件打开失败";
            return;
        }
        else
        {
            file.open(QIODevice::WriteOnly);
            QTextStream qs(&file);
            for(int i = 0;i<length;i++)
            {
                qs <<y1[i];
                //qs << QString::number(y1[i]);
            }
            file.close();
        }

    }

    emit work_finished(y1);
}

void worker::receiveData(char* current_buffer,QString data_type, int length)//TODO:需要修改
{
    this->y.clear();
    if(data_type =="long")
    {

        //从xy两个大数组中提取一帧画面所需要的数据
        int datatype_room = sizeof(long); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            this->y.append(*((long*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
        }
    }
    else if (data_type == "short") {
        int datatype_room = sizeof(short); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            this->y.append(*((short*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
            //qDebug() <<i << *((short*)(current_buffer));
        }

    }
    else if (data_type == "int")
    {
        int datatype_room = sizeof(int); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            y.append(*((int*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
        }

    }
    else if (data_type == "char *")
    {
        int datatype_room = sizeof(char); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            y.append(*((char*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
        }

    }
    else if (data_type == "float")
    {
        int datatype_room = sizeof(float); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            y.append(*((float*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
        }
    }
    else if (data_type == "double")
    {
        int datatype_room = sizeof(double); //记录数据类型所占用的内存空间
        int point_num_max=(length)/datatype_room;
        for(int i =0;i<point_num_max;i++)
        {
            y.append(*((double*)(current_buffer)));
            current_buffer = current_buffer + datatype_room;
        }

    }

}
