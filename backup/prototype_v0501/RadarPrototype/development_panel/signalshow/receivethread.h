#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H

#include <QObject>
#include <QThread>

class ReceiveThread : public QThread
{
    Q_OBJECT
public:
    void run();
    ReceiveThread();

    int getReceivedata_interval() const;
    //修改间隔
    void setReceivedata_interval(int value);

signals:
    void sendAddressAndLen(char *array, int len);

private:
    // 接收数据的间隔,中间件发送几组数据后接受一组
    int receivedata_interval = 1;
};

#endif // RECEIVETHREAD_H
