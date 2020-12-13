#include "itemport.h"
#include <QGraphicsScene>
#include <QFontMetrics>

#include <QPen>
#include "arrow.h"
#include "diagramitem.h"
#include <QDebug>

ItemPort::ItemPort(QString name_, bool isOutput, QGraphicsItem *parent):
    QGraphicsPathItem(parent), name(name_), isOutput_(isOutput)
{
    label = new QGraphicsTextItem(this);
    label->setPlainText(name);

    radius_ = 5;
    margin = 2;

//    QFontMetrics fm(scene()->font());
//	QRect r = fm.boundingRect(name);

    QPainterPath p;
    QPolygonF poly;
    //根据是否为输入确定端口绘制位置
    if(isOutput_)
    {
        // 左上角坐标
        label->setPos(-radius_ - margin - label->boundingRect().width(), -label->boundingRect().height() / 2);
        poly << QPointF(-radius_, 0) << QPointF(0, 0) << QPointF(0, radius_) << QPointF(2 * radius_, 0) << QPointF(0, -radius_) << QPointF(0, 0)
             << QPointF(-radius_, 0);
    }
    else
    {
        label->setPos(radius_ + margin, -label->boundingRect().height() / 2);
        poly << QPointF(-radius_, 0) << QPointF(-radius_, radius_) << QPointF(radius_, 0) << QPointF(-radius_, -radius_) << QPointF(-radius_, 0)
             << QPointF(-radius_, 0);

    }
    p.addPolygon(poly);
//    p.addEllipse(-radius_, -radius_, 2*radius_, 2*radius_);
    setPath(p);

    setPen(QPen(Qt::darkGray));
    setBrush(Qt::darkGray);
    //位置改变会发送通知，啥通知？
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

    m_portFlags = 0;//仅在显示字体上用到？？
}

// 对象销毁操作Qt应该自带的
//ItemPort::~ItemPort()
//{
//    foreach(Arrow *conn, m_connections)
//        delete conn;
//}

void ItemPort::setItemBlock(DiagramItem *b)
{
    m_block = b;
}

//void ItemPort::setName(const QString &n)
//{
//    name = n;
//    label->setPlainText(n);
//}

//void ItemPort::setIsOutput(bool o)
//{
//    isOutput_ = o;

//    QFontMetrics fm(scene()->font());

//    if (isOutput_)
//        // 左上角坐标
//        label->setPos(-radius_ - margin - label->boundingRect().width(), -label->boundingRect().height()/2);
//    else
//        label->setPos(radius_ + margin, -label->boundingRect().height()/2);
//}

int ItemPort::radius()
{
    return radius_;
}

bool ItemPort::isOutput()
{
    return isOutput_;
}

void ItemPort::addArrow2Connections(Arrow *a)
{
    this->m_connections.push_back(a);
}

//QList<Arrow *> &ItemPort::connections()
//{
//    return m_connections;
//}

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-09-29 T 15:15:35
 * @copyright (c)
 * @brief ItemPort::setPortFlags 设置旗帜，是否是斜体或者加粗显示
 * @param f flag判断是什么类型
 */
void ItemPort::setPortFlags(int f)
{
    m_portFlags = f;

    if (m_portFlags & TypePort)
    {
//        QFont font(scene()->font());
        QFont font;
        // 斜体
        font.setItalic(true);
        label->setFont(font);
        // 空路径
        setPath(QPainterPath());
    }
    else if (m_portFlags & NamePort)
    {
//        QFont font(scene()->font());
        QFont font;
        // 加粗
        font.setBold(true);
        font.setPixelSize(15);
        label->setFont(font);
        setPath(QPainterPath());
    }
    else if(m_portFlags & ParamPort)
    {
        QFont font;
        font.setBold(true);
        // 斜体
        font.setItalic(true);
        label->setFont(font);
        setPath(QPainterPath());
    }
}

DiagramItem *ItemPort::block() const
{
    return m_block;
}

// 遍历此this端口上的所有线段，是否有连接到另外那个端口other的
bool ItemPort::isConnected(ItemPort *other)
{
    foreach(Arrow *conn, m_connections)
        if (conn->port1() == other || conn->port2() == other)
            return true;

    return false;
}

void ItemPort::removeConnection(Arrow *arrow)
{
    m_connections.removeOne(arrow);
}

QVariant ItemPort::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // 位置有变动
    if (change == ItemScenePositionHasChanged)
    {
//        qDebug() << "m_connections" << m_connections.size();
        if(m_connections.size() > 0)
        {
            // 所有相连的线段都得更新位置
            foreach(Arrow *conn, m_connections)
            {
//                qDebug() << "conn" << conn;
                // 先更新端口
                conn->updatePosFromPorts();
                // 再更新位置
                conn->updatePath();
            }
        }
    }
    return value;
}

int ItemPort::getPortFlags() const
{
    return m_portFlags;
}

bool ItemPort::getIsOutput() const
{
    return isOutput_;
}

void ItemPort::setIsOutput(bool isOutput)
{
    isOutput_ = isOutput;
}

QString ItemPort::getDataType() const
{
    return dataType;
}

void ItemPort::setDataType(const QString &value)
{
    dataType = value;
}

QList<Arrow *> ItemPort::getConnections() const
{
    return m_connections;
}

QString ItemPort::getUuid() const
{
    return uuid;
}

void ItemPort::setUuid(const QString &value)
{
    uuid = value;
}
