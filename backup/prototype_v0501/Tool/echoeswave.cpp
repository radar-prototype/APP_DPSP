#include "echoeswave.h"
#include "ui_echoeswave.h"
#include <QVector>
#include <QTimer>
#include <QTime>

EchoesWave::EchoesWave(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EchoesWave)
{
    ui->setupUi(this);
    setupRealtimeDataDemo(ui->customPlot);
    ui->customPlot->replot();
    ui->checkBox_temp->setChecked(true);
}

//画图初始化
void EchoesWave::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->graph(0)->setName("wave");
  //customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  //customPlot->graph(0)->setAntialiasedFill(false);
  //customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));


  customPlot->xAxis->setTicks(true);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  //connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  //connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
  customPlot->legend->setVisible(true);
}

void EchoesWave::realtimeDataSlot()
{
    //key的单位是秒
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    qsrand(QTime::currentTime().msec() + QTime::currentTime().second() * 10000);
    //使用随机数产生两条曲线
    double value0 = qrand() % 100;
//    double value1 = qrand() % 80;
    if (ui->checkBox_temp->isChecked())
        ui->customPlot->graph(0)->addData(key, value0);//添加数据1到曲线1

    //自动设定graph(1)曲线y轴的范围，如果不设定，有可能看不到图像
    //也可以用ui->customPlot->yAxis->setRange(up,low)手动设定y轴范围

    ui->customPlot->graph(0)->rescaleValueAxis();

    //这里的8，是指横坐标时间宽度为8秒，如果想要横坐标显示更多的时间
    //就把8调整为比较大到值，比如要显示60秒，那就改成60。
    ui->customPlot->xAxis->setRange(key+0.25, 60, Qt::AlignRight);//设定x轴的范围
    ui->customPlot->replot();
}


EchoesWave::~EchoesWave()
{
    delete ui;
}





