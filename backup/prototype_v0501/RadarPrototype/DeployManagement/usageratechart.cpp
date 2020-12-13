#include "usageratechart.h"
#include <QPainter>
#include <QDebug>

UsageRateChart::UsageRateChart(QWidget *parent)
    : QWidget(parent)
{
    qsrand(2);
    mHeight = this->height();
    mWidth = this->width();
    center = QPointF(mWidth / 2,mHeight / 2);
    chartWidth = mWidth - margin * 2;
    chartHeight = mHeight - margin * 2;
    basePoint = QPointF(mWidth - margin,mHeight - margin);
    horizontalUnit = chartWidth / 20;
    verticalUnit = chartHeight / 10;
    bias = 0;
    update();
}

UsageRateChart::~UsageRateChart()
{

}

void UsageRateChart::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    // 绘制方格
    painter.drawRect(margin,margin,chartWidth,chartHeight);
    // 横线
    painter.setPen(Qt::DotLine);
    for(int i = 1;i < 10;++i)
    {
        painter.drawLine(margin, margin + verticalUnit * i,mWidth - margin,margin + verticalUnit * i);
    }
    // 竖线
    for(int i = 1;i < 21;++i)
    {
        painter.drawLine(margin + horizontalUnit * (i - (qreal)bias / 3),margin,margin + horizontalUnit * (i - (qreal)bias / 3),mHeight - margin);
    }
    // 文字说明
    painter.setPen(Qt::black);
    QFontMetrics fm = painter.fontMetrics();
    painter.drawText(basePoint.x(),basePoint.y() + fm.ascent(),QStringLiteral("0"));
    painter.drawText(margin - 2 * fm.descent(),mHeight - margin + fm.ascent(),QStringLiteral("60秒"));
    painter.drawText(mWidth - margin,margin + fm.ascent(),QStringLiteral("100%"));
    // 折线

    QPainterPath path(basePoint);
    //if(points.size() >= 60)
        //path.lineTo(QPointF(margin,mHeight - margin));
    path.lineTo(points[0].x(),mHeight - margin);

    for(auto point : points)
    {
        path.lineTo(point);
    }

    painter.setBrush(QColor().fromRgb(241, 246, 250,150));
    painter.setPen(QColor().fromRgb(39, 137, 193));
    painter.drawPath(path);

}

/*
* fanzhouyang
* 获取新的数据，并更新图表
*/

void UsageRateChart::updateChart(QVector<QPointF> &vector)
{
    points.clear();

    for(QPointF point:vector)
    {
        points.append(pos(point));
    }

    bias = (bias + 1) % 3;
    update();
}

/*
* fanzhouyang
* 由（时间，利用率）产生图表中对应位置的坐标
*/

QPointF UsageRateChart::pos(QPointF point)
{
    return QPointF(basePoint.x() - point.x() * horizontalUnit / 3,basePoint.y() - point.y() * verticalUnit / 10);
}

/*
* fanzhouyang
* resize
*/

void UsageRateChart::resizeEvent(QResizeEvent *)
{
    mHeight = this->height();
    mWidth = this->width();
    center = QPointF(mWidth / 2,mHeight / 2);
    chartWidth = mWidth - margin * 2;
    chartHeight = mHeight - margin * 2;
    basePoint = QPointF(mWidth - margin,mHeight - margin);
    horizontalUnit = chartWidth / 20;
    verticalUnit = chartHeight / 10;
    update();
}
