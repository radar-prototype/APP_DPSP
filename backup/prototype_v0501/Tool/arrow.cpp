#include "arrow.h"
#include <math.h>
#include <QPen>
#include <QPainter>
#include <QMenu>
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QDataStream>
#include "radarscene.h"

const qreal Pi = 3.14;

/**
* @projectName   prototype_v0719
* @brief         用于编辑雷达页面的组件元素端口之间链接的箭头类。
* @author        Antrn
* @date          2019-08-12
*/
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem, QGraphicsItem *parent): QGraphicsPathItem(parent)
{
    myStartItem = startItem;
    myEndItem = endItem;
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

Arrow::Arrow(QGraphicsItem *parent): QGraphicsPathItem(parent)
{
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(Qt::NoBrush);
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable);
    m_port1 = nullptr;
    m_port2 = nullptr;
}

//Arrow::~Arrow()
//{
//    if (m_port1)
//        m_port1->connections().removeAt(m_port1->connections().indexOf(this));
//    if (m_port2)
//        m_port2->connections().removeAt(m_port2->connections().indexOf(this));
//}
//右键点击箭头出现菜单
void Arrow::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    setSelected(true);
    // 鼠标是不是在场景里
    bool is = dynamic_cast<RadarScene*>(this->scene())->isSelected;
    if(!is)
    {
        dynamic_cast<RadarScene*>(this->scene())->setIsSelected(true);
        qDebug() << "isSelected设置为true，场景被选中";
    };

    Arrow_Menu = new QMenu();
    deleteAction=new QAction(tr("删除"),this);
    connect(deleteAction,SIGNAL(triggered()),this,SLOT(delete_Arrow()));
    showAction=new QAction(tr("属性"),this);
    connect(showAction,SIGNAL(triggered()),this,SLOT(show_Arrow()));

    Arrow_Menu->addAction(deleteAction);
    Arrow_Menu->addAction(showAction);
    Arrow_Menu->exec(event->screenPos());
}
void Arrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setSelected(true);
    QGraphicsItem::mousePressEvent(event);
}

void Arrow::delete_Arrow()
{
    //传递删除箭头信号到radarscene
    emit deletearrow_signal(this->itemId);
}

void Arrow::show_Arrow()
{
    //保存箭头的相关信息，包括端口和组件信息
    Arrow_Data arrow_data;
    arrow_data.begin_comp=this->myStartItem->iconName;
    arrow_data.begin_port=this->m_port1->portName();
    arrow_data.end_comp=this->myEndItem->iconName;
    arrow_data.end_port=this->m_port2->portName();
    arrow_data.ID=this->itemId;

    QVariant variant;
    variant.setValue(arrow_data);

    //将箭头的相关信息传递到属性窗口
    Arrow_property.setArrow(variant);
    Arrow_property.show();
}
void Arrow::setPos1(const QPointF &p)
{
    pos1 = p;
}

void Arrow::setPos2(const QPointF &p)
{
    pos2 = p;
}

void Arrow::setPort1(ItemPort *p)
{
    m_port1 = p;

    m_port1->addArrow2Connections(this);
}

void Arrow::setPort2(ItemPort *p)
{
    m_port2 = p;

    m_port2->addArrow2Connections(this);
}

void Arrow::updatePosFromPorts()
{
    // 重新获取两端的端口的场景中的位置
    pos1 = m_port1->scenePos();
    pos2 = m_port2->scenePos();
}

void Arrow::updatePath()
{
    QPainterPath curr_p;
//    qreal arrowSize = 10;
    // 起点
    curr_p.moveTo(pos1);

    // TODO 当起点比终点还靠右的情况
    qreal dx = pos2.x() - pos1.x();
    qreal dy = pos2.y() - pos1.y();

    // 画的是个曲线
    QPointF ctr1(pos1.x() + dx * 0.25, pos1.y() + dy * 0.1);
    QPointF ctr2(pos1.x() + dx * 0.75, pos1.y() + dy * 0.9);

    curr_p.cubicTo(ctr1, ctr2, pos2);

//    QPointF arrowP1 = pos2 + QPointF(-10,2);
//    QPointF arrowP2 = pos2 + QPointF(-10,-2);

//    curr_p.lineTo(arrowP1);
//    curr_p.lineTo(arrowP2);
//    curr_p.lineTo(pos2);
//    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
//    double random = double(qrand()%7+2)/double(10);
//    QPointF ctr1(pos1.x() + dx * random, pos1.y());
//    QPointF ctr2(pos1.x() + dx * random, pos2.y());

//    curr_p.lineTo(ctr1);
//    curr_p.lineTo(ctr2);
//    curr_p.lineTo(pos2);

    // 就能更新
    setPath(curr_p);
}

ItemPort *Arrow::port1() const
{
    return m_port1;
}

ItemPort *Arrow::port2() const
{
    return m_port2;
}

//QRectF Arrow::boundingRect() const
//{
//    qreal extra = (pen().width() + 20) / 2.0;

//    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
//                                      line().p2().y() - line().p1().y()))
//        .normalized()
//        .adjusted(-extra, -extra, extra, extra);
//}

//QPainterPath Arrow::shape() const
//{
//    QPainterPath path = QGraphicsLineItem::shape();
//    path.addPolygon(arrowHead);
//    return path;
//}

//void Arrow::updatePosition()
//{
//    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
//    setLine(line);
//}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem * option,
                  QWidget *)
{
//    //碰撞检测，不画箭头
//    if (myStartItem->collidesWithItem(myEndItem))
//        return;

//    QPen myPen = pen();
//    //myColor 成员变量，记录颜色
//    myPen.setColor(myColor);
//    qreal arrowSize = 10;
    painter->setRenderHint(QPainter::Antialiasing, true);
//    painter->setPen(myPen);
//    painter->setBrush(myColor);

//    //传入在视图中的两个坐标为起始和终点，分别为中点
//    QLineF centerLine(myStartItem->pos(), myEndItem->pos());
//    QPolygonF endPolygon = myEndItem->polygon();
//    QPointF p1 = endPolygon.first() + myEndItem->pos();
//    QPointF p2;
//    QPointF intersectPoint;
//    QLineF polyLine;
//    //从第二个点开始遍历，1,2,3,4，共5个点
//    for (int i = 1; i < endPolygon.count(); ++i) {
//        p2 = endPolygon.at(i) + myEndItem->pos();
//        polyLine = QLineF(p1, p2);
//        //判断两个item的连线和此条边界线是否相交，记录相交类型
//        QLineF::IntersectType intersectType =
//            polyLine.intersect(centerLine, &intersectPoint);
//        //在线段内直接相交
//        if (intersectType == QLineF::BoundedIntersection)
//            break;
//        p1 = p2;
//    }

//    // 重新设置线段，箭头在边界上
//    setLine(QLineF(intersectPoint, myStartItem->pos()));


//    double angle = ::acos(line().dx() / line().length());
//    if (line().dy() >= 0)
//        angle = (Pi * 2) - angle;

//    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
//                                    cos(angle + Pi / 3) * arrowSize);
//    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
//                                    cos(angle + Pi - Pi / 3) * arrowSize);

//    arrowHead.clear();
//    arrowHead << line().p1() << arrowP1 << arrowP2;

//    painter->drawLine(line());
//    painter->drawPolygon(arrowHead);

    painter->setBrush(Qt::NoBrush);
    //箭头被选中
    if (option->state & QStyle::State_Selected)
    {
        painter->setPen(QPen(Qt::red, 2, Qt::DashLine));
        QPainterPath p = this->path();
        painter->drawPath(p);
    }
    else
    {
        painter->setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(this->path());
    }
}

void Arrow::setMyEndItem(DiagramItem *value)
{
    myEndItem = value;
}

void Arrow::setMyStartItem(DiagramItem *value)
{
    myStartItem = value;
}
