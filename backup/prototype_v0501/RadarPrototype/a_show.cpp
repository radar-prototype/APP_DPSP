#include "a_show.h"
#include "ui_a_show.h"

#include <QDateTime>
#include <QPainter>
#include <QTimer>

A_show::A_show(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::A_show)
{
    ui->setupUi(this);
    p = t = 0;
    timer = new QTimer;
    path = new QPainterPath;
    point = new QPoint;
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
}

A_show::~A_show()
{
    delete ui;
    delete timer;
    delete point;
    delete path;
}

void A_show::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(QColor(255, 255, 255, 255),1));

    painter.setBrush(Qt::black);
    painter.drawRect(0,0,850,300);

//    for(int i = 0; i < 855; i += 15)
//        painter.drawLine(i + 0.5, 0, i + 0.5, 300);
//    for(int j = 0; j < 300; j += 15)
//        painter.drawLine(0, j + 0.5, 850, j + 0.5);

    painter.setPen(QPen(Qt::green)); //adjust the painter size and color
    QPoint originPoint(p,0);
    painter.translate(originPoint);  //adjust the origin point of the painting
    painter.drawPath(*path);  //painting

    painter.setPen(Qt::white);
    // 在信号之后绘制中间白线，并随着向左移动，起始坐标实时变化
    painter.drawLine(-p, 150, -p+855, 150);
    QWidget::paintEvent(event);
}

void A_show::timerUpdate()
{
    t += 10;
    //create random number
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int rand=qrand()%this->height();
    //the next point of the painted line
    point->setX(t);
    point->setY(rand);
    path->lineTo(*point);
    if(t > this->width())
    {
        p -= 10;
    }

    //in case update() method is called, the system will trigger paintEvent to repaint the window
    this->update();
}

void A_show::startPainting()
{
    timer->start(100);
}

void A_show::stopPainting()
{
    timer->stop();
}


