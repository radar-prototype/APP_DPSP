#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QPen>
/**
* @projectName   prototype_v0719
* @brief         用于新建工程面板的雷达/对抗/目标 间链接的箭头类。（总体仿真箭头）
* @author        Antrn
* @date          2019-08-12
*/
class ArrowItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    const static float LINE_WIDTH;
    const static float ARROW_SIZE;

    ArrowItem();
    void setData(const QPointF& from,const QPointF& to);
    void setLineItem(QGraphicsItem*begin,QGraphicsItem*end);
    QGraphicsItem*getBeginItem();
    QGraphicsItem*getEndItem();

    QPainterPath shape()const;//重写shape函数
    QRectF boundingRect()const;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* style, QWidget* widget);

public slots:
    void delete_item();
    void show_property();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    bool mValid;
    QPointF mFrom;
    QPointF mTo;

    QPointF mP1;
    QPointF mP2;

    QGraphicsItem*line_begin_item;
    QGraphicsItem*line_end_item;


    int ellipse_r = 0;
    QPen pen;
    QBrush brush;
    QPolygonF arrowHead;
};

#endif // ARROWITEM_H
