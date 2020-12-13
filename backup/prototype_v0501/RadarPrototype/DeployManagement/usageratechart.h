#ifndef USAGERATECHART_H
#define USAGERATECHART_H

/*
* fanzhouyang
* QCharts不被支持，用QPainter重新实现硬件资源利用率变化的折线图
*/

#include <QWidget>
#include <QTimer>

class UsageRateChart : public QWidget
{
    Q_OBJECT

public:
    UsageRateChart(QWidget *parent = nullptr);
    ~UsageRateChart();
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void updateChart(QVector<QPointF> &);
    QPointF pos(QPointF);

private:
    int margin = 45;
    int bias;
    qreal mHeight;
    qreal mWidth;
    qreal chartWidth;
    qreal chartHeight;
    qreal horizontalUnit;
    qreal verticalUnit;
    QPointF center;
    QPointF basePoint;
    QVector<QPointF> points;
};

#endif // USAGERATECHART_H
