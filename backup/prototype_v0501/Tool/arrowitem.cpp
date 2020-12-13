#include "arrowitem.h"
#include "mainwindownewscene.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QMenu>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <math.h>
const float ArrowItem::LINE_WIDTH = 2.0f;
const float ArrowItem::ARROW_SIZE = 8.0f;
static const double PI = 3.14159265358979323846264338327950288419717;
/**
* @projectName   prototype_v0719
* @brief         用于新建工程面板的雷达/对抗/目标 之间链接的箭头类。
* @author        Antrn
* @date          2019-08-12
*/
ArrowItem::ArrowItem(): mValid(false)
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    //设置可拖动
//    setFlag(QGraphicsItem::ItemIsMovable);
}

void ArrowItem::setLineItem(QGraphicsItem*begin, QGraphicsItem*end)
{
    line_begin_item = begin;
    line_end_item = end;
}
QGraphicsItem*ArrowItem::getBeginItem()
{
    return line_begin_item;
}
QGraphicsItem*ArrowItem::getEndItem()
{
    return line_end_item;
}


void ArrowItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu *menu = new QMenu;
    menu->addAction(tr("删除"), this, SLOT(delete_item()));
    menu->addAction(tr("属性"), this, SLOT(show_property()));
    menu->exec(event->screenPos());
    delete menu;
}

void ArrowItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setSelected(true);
    QGraphicsItem::mousePressEvent(event);
}

void ArrowItem::setData(const QPointF& from, const QPointF& to)
{
    mValid = true;
    mFrom = from;
    mTo = to;

    QLineF line(mFrom, mTo);
    setLine(line);
    //计算角度
    qreal angle = ::acos(line.dx() / line.length());
//    qDebug() << "角度: " << angle;

    if(line.dy() >= 0)
        angle = PI * 2 - angle;

    mP1 = mTo + QPointF(sin(angle - PI / 3) * ARROW_SIZE, cos(angle - PI / 3) * ARROW_SIZE);
    mP2 = mTo + QPointF(sin(angle - PI + PI / 3) * ARROW_SIZE, cos(angle - PI + PI / 3) * ARROW_SIZE);
}

QPainterPath ArrowItem::shape() const
{
    QPainterPath trianglePath = QGraphicsLineItem::shape();
    trianglePath.addPolygon(arrowHead);
//    trianglePath.moveTo(mFrom);
//    trianglePath.lineTo(50,50);
//    trianglePath.lineTo(-50,50);
//    trianglePath.closeSubpath();
    return trianglePath;
}

QRectF ArrowItem::boundingRect()const
{
    qreal extra = (LINE_WIDTH + ARROW_SIZE) / 2.0;
    QRectF rect = QRectF(mFrom, QSizeF(mTo.x() - mFrom.x(), mTo.y() - mFrom.y())).normalized().
                  adjusted(-extra, -extra, extra, extra);
    return rect;
}

void ArrowItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    if(!mValid)
        return;
    painter->setRenderHint(QPainter::Antialiasing);
    QPen p(QColor::fromRgb(79, 136, 187));
    painter->setBrush(QBrush(p.color()));
    p.setWidthF(LINE_WIDTH);
    painter->setPen(p);
    painter->drawLine(mFrom, mTo);
    painter->drawPolygon(QPolygonF() << mTo << mP1 << mP2);
    arrowHead.clear();
    arrowHead << mTo << mP1 << mP2;

    if (isSelected())
    {
        qDebug() << "箭头被选择";
        painter->setPen(QPen(QColor::fromRgb(79, 136, 187), 1, Qt::DashLine));
        QLineF myLine = line();
        //往右平移4
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        //再往左平移8，这样两边各为4
        myLine.translate(0, -8.0);
        painter->drawLine(myLine);
    }
}

void ArrowItem::delete_item()
{
    this->destroyed();
}

void ArrowItem::show_property()
{
    // TODO 箭头没有属性也无所谓
    qDebug() << "show arrow's property";
}

