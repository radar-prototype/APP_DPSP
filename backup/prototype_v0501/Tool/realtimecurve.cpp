#include "realtimecurve.h"
#include "ui_realtimecurve.h"

#include <QDateTime>
#include <QFile>
#include <QPainter>
#include <QTextStream>
#include <QTimer>
#include <QDebug>

RealTimeCurve::RealTimeCurve(QString p, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RealTimeCurve),
    fp(p)
{
    ui->setupUi(this);
    this->resize(600,500);      //resize the main window
    ui->startButton->setStyleSheet("background: white");
    ui->stopButton->setStyleSheet("background: white");
    p = t = 0;
    timer = new QTimer;
    path = new QPainterPath;
    point = new QPoint;
    qDebug() << "数据文件路径" << fp;
    readFile(fp);
    connect(ui->startButton, SIGNAL(clicked()),this,SLOT(startPainting()));
    connect(ui->stopButton, SIGNAL(clicked()),this,SLOT(stopPainting()));
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));

}

RealTimeCurve::~RealTimeCurve()
{
    delete ui;
    delete timer;
    delete point;
    delete path;
}

void RealTimeCurve::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::green)); //adjust the painter size and color
    QPoint originPoint(p,0);
    painter.translate(originPoint);  //adjust the origin point of the painting
    painter.drawPath(*path);  //painting
    QWidget::paintEvent(event);
}

void RealTimeCurve::readFile(QString filePath){
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "文件打开失败";
        return;
    }
    QTextStream in(&file);
    QString line = in.readLine();
    int num=0;
    while (!line.isNull()){
        line = in.readLine();
        double d = line.split('\t')[0].toDouble(); // 取出实部real
        realArray.append(d); // WARNING 如此大的数据，内存够吗，看你配置了
        num++;
    }
    qDebug() << "总共行数: " << num;
}


void RealTimeCurve::timerUpdate()
{
    if(t>=realArray.size()){
        timer->stop();
        return;
    }
//    t += 1;
    //create random number
//    QTime time;
//    time= QTime::currentTime();
//    qsrand(time.msec()+time.second()*1000);
//    int rand=qrand()%this->height();
    //the next point of the painted line
    point->setX(t);
    point->setY(realArray[t++]);
    path->lineTo(*point);
    if(t > this->width())
    {
        p -= 1;
    }

    //in case update() method is called, the system will trigger paintEvent to repaint the window
    this->update();
}

void RealTimeCurve::startPainting()
{
    timer->start(1);
}

void RealTimeCurve::stopPainting()
{
    timer->stop();
}
