#include "echoeswave.h"
#include "qudong.h"
#include "runcompconf.h"
#include "ui_runcompconf.h"
#include <QDebug>
#include <QTimer>
RunCompConf::RunCompConf(QString fp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunCompConf),
    dataPath(fp)
{
    ui->setupUi(this);
    connect(ui->btn_start, &QPushButton::clicked, this, &RunCompConf::On_start);
    connect(this, SIGNAL(startRun()), this, SLOT(On_start()));
    connect(this, SIGNAL(rateSignal(float)), this, SLOT(On_rateSignal(float)));
    connect(this, SIGNAL(overRun()), this, SLOT(On_over()));
    connect(ui->btn_pause, SIGNAL(clicked()), this, SLOT(On_pause()));
    connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(On_over()));

    ui->run_progressBar->setValue(0);
    ui->btn_stop->setEnabled(false);
    ui->btn_pause->setEnabled(false);
//    QDir dir(QDir::currentPath());
//    dir.cdUp();
//    dir.cd(qAppName()+"/Projects/test/code");
//    QString workpath = dir.absolutePath();
//    qDebug() << "workpath" << workpath;
//    readFile(workpath+"/result.txt");
    readFile(dataPath);
}

RunCompConf::~RunCompConf()
{
    delete ui;
}

void RunCompConf::On_start()
{
    qDebug() << "Start";
    ui->btn_start->setEnabled(false);
    ui->btn_pause->setEnabled(true);
    ui->run_progressBar->setValue(0);

//    QTimer::singleShot( 0, this, [=](){
//        emit rateSignal(10);}
//    );
#if 0
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd(qAppName() + "/Projects/test/code");
    qDebug() << "On_start here" << dir;
    QString workpath = dir.absolutePath();
    // WARNING 执行指令，这个指令记得及时修改
    QuDong::startRun(workpath + "/radar.c", workpath + "/radar.out");
#endif
//    QTimer::singleShot( 1000, this, [=](){
//        emit rateSignal(70);}
//    );
//    EchoesWave *ew = new EchoesWave;
//    ew->show();
    setupRealtimeDataDemo(ui->customPlot);
    ui->customPlot->replot();
//    QTimer::singleShot( 3000, this, [=](){
//        emit rateSignal(100);}
//    );
//    emit overRun();
}


//画图初始化
void RunCompConf::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setName("wave");
    //customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    //customPlot->graph(0)->setAntialiasedFill(false);
    //customPlot->graph(0)->setChannelFillGraph(customPlot->graph(0));

    customPlot->legend->setVisible(true);
    customPlot->xAxis->setTicks(true);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // make left and bottom axes transfer their ranges to right and top axes:
    //connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    //connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
//  realtimeDataSlot();
}
// This is an auto-generated comment.
/**
* @projectName   prototype_v0501
* @brief         简介 读取result.txt处理之后的文件，存入realArray数组中
* @author        Antrn
* @date          2019-12-02
*/
void RunCompConf::readFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString line = in.readLine();
    int num = 0, lines = 16 * 4096;
    while (!line.isNull() && num < lines)
    {
        line = in.readLine();
        double d;
        if(line.contains("\t"))
            d = line.split('\t')[0].toDouble(); // 取出实部real
        else
        {
            d = line.toDouble();
        }
        realArray.append(d); // WARNING 如此大的数据，内存够吗
        num++;
    }
    qDebug() << "total rows: " << num;
}
/**
 * @brief RunCompConf::realtimeDataSlot
 */
void RunCompConf::realtimeDataSlot()
{
    int lines = realArray.size();

    if(i == lines)
    {
        qDebug() << "[runcompconf.cpp]数据到头： 退出！！";
        dataTimer.stop();
        emit rateSignal(100);
        return;
    }

    // ASSERT failure in QVector<T>::operator[]: "index out of range"
//    if(lines > 0){
//        qDebug() << "lines: " << lines << "i: " << i << "; real: " << realArray[i];
//    }

    ui->customPlot->graph(0)->addData(i, realArray[i]);//添加数据1到曲线1
//    emit rateSignal(i*100.0/(65536));
    emit rateSignal(i * 100 / (lines));
    i++;


    //自动设定graph(0)曲线y轴的范围，如果不设定，有可能看不到图像
    //也可以用ui->customPlot->yAxis->setRange(up,low)手动设定y轴范围

    ui->customPlot->graph(0)->rescaleValueAxis();

    //这里的8，是指横坐标时间宽度为8秒，如果想要横坐标显示更多的时间
    //就把8调整为比较大到值，比如要显示60秒，那就改成60。
    ui->customPlot->xAxis->setRange(i, 9600, Qt::AlignRight);//设定x轴的范围
    ui->customPlot->replot();
}

void RunCompConf::On_pause()

{
    qDebug() << "Pause";
    ui->btn_start->setEnabled(false);
    dataTimer.stop();
}

void RunCompConf::On_over()
{
    qDebug() << "Done";
    dataTimer.stop();
    // TODO 能够将进程杀死
    if(dataTimer.isActive())
    {
        killTimer(dataTimer.timerId());
    }
    ui->btn_start->setEnabled(false);
    ui->btn_pause->setEnabled(false);
}

void RunCompConf::On_rateSignal(float rate)
{
//    qDebug() << "进度：" << rate;
    ui->run_progressBar->setValue(int(rate));
}

