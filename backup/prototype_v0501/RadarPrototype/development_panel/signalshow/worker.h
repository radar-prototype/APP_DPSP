#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QVector>
//weishi:用于处理求模的计算线程
class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = nullptr);

    void receiveData(char* , QString, int);
    QVector<double> y;


signals:
    void work_finished(QVector<double> res);//线程的结束信号

public slots:
    void do_work(char *, int, QString, QString, int, bool);//用来传递主界面传过来的数组进行求模


};

#endif // WORKER_H
