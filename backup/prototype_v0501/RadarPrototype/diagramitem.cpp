#include "diagramitem.h"
#include "arrow.h"
#include "radarscene.h"
#include<QGraphicsScene>
#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QFileInfo>
#include <utils.h>

/**
* @projectName   prototype_v0719
* @brief         这里就是悬浮框中雷达内部组件的图表类。
* @author        Antrn
* @date          2019-08-12
*/
DiagramItem::DiagramItem(QMenu *contextMenu, QGraphicsItem *parent): QGraphicsPathItem(parent)
{
    myContextMenu = contextMenu;
//    QString s = QDir::currentPath()+"/radar/"+getRadar_id()+"/images/";
//    Utils::openDirOrCreate(s);
//    QFileInfo fi(s+iconName+".ico");
//    if(!fi.isFile()){
//        bool tof = QFile::copy(QDir::currentPath()+"/images/"+iconName+".ico", s+iconName+".ico");
//        qDebug() << "文件复制成功与否：" << tof;
//    }
    comp = new CompProperty(iconName);
    QPainterPath p; //QPainterPath类为绘制操作提供了一个容器，允许构造和重用图形形状。
    p.addRoundedRect(-50, -15, 100, 30, 5, 5);
    setPath(p);
    setPen(QPen(Qt::darkGray));
    setBrush(Qt::white);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    horzMargin = 20;
    vertMargin = 5;
    width = horzMargin;
    height = vertMargin;

    setAcceptHoverEvents(true);
    // 坐标变化
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

DiagramItem::DiagramItem(QString itemUuid, QString iconName, QMenu *contextMenu, QString radarId, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), itemSuuid(itemUuid), iconName(iconName), radar_id(radarId)
{
    myContextMenu = contextMenu;
//    QString s = QDir::currentPath()+"/radar/"+getRadar_id()+"/images/";
//    Utils::openDirOrCreate(s);
//    QFileInfo fi(s+iconName+".ico");
//    if(!fi.isFile()){
//        bool tof = QFile::copy(QDir::currentPath()+"/images/"+iconName+".ico", s+iconName+".ico");
//        qDebug() << "文件复制成功与否：" << tof;
//    }
    this->setToolTip("Name: " + iconName + ";\nId: " + itemSuuid);

    // 统一形状
//    myPolygon << QPointF(-50, -50) << QPointF(50, -50)
//          << QPointF(50, 50) << QPointF(-50, 50)
//          << QPointF(-50, -50);
//    setPolygon(myPolygon);

    comp = new CompProperty(iconName);
    QPainterPath p;
    p.addRoundedRect(-50, -15, 100, 30, 5, 5);
    setPath(p);
    setPen(QPen(Qt::darkGreen));
    setBrush(Qt::white);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    horzMargin = 20;
    vertMargin = 10;
    width = horzMargin;
    height = vertMargin;

    setAcceptHoverEvents(true);
    // 坐标变化
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

/**
 * @brief DiagramItem::addPort 添加端口
 * @param name 端口的名字
 * @param isOutput 是不是输出端口
 * @param flags 端口flags，作用：
 * @param ptr 指针
 * @return 端口对象
 */
ItemPort *DiagramItem::addPort(const QString &name, QString id, bool isOutput, int flags, QString dt)
{
    ItemPort *port = new ItemPort(name, isOutput, this);
//    port->setName(name);
    port->setItemBlock(this);
    port->setPortFlags(flags);
    port->setDataType(dt);
    port->setUuid(id);
    addPort2mports(id, port);

//    QFontMetrics fm(scene()->font());
    QFont font;
    font.setPixelSize(20);
    QFontMetrics fm(font);//提供字体量度信息
    int w = fm.width(name) + 15; //端口名字的宽度
    int h = fm.height() + 5; //一行的高度
    if (w > width - horzMargin)
        width = w + horzMargin;
    height += h;

    QPainterPath p;
    p.addRoundedRect(-width / 2, -height / 2, width, height, 5, 5);
//    p.addPolygon(QPolygonF() << QPointF(-width / 2, 8) << QPointF(width / 2, 8));
    setPath(p);

    int y = -height / 2 + vertMargin + port->radius();
    foreach(QGraphicsItem *port_, childItems())
    {
        if (port_->type() != ItemPort::Type)
            continue;

        // QNEPort *port = (QNEPort*) port_;
        ItemPort *port = qgraphicsitem_cast<ItemPort*>(port_);
        if (port->isOutput())
            port->setPos(width / 2 + port->radius(), y);
        else
            port->setPos(-width / 2 - port->radius(), y);
        y += h;
    }

    return port;
}

void DiagramItem::addInputPort(const QString &name, QString id, QString datatype)
{
    addPort(name, id, false, 0, datatype);
}

void DiagramItem::addOutputPort(const QString &name, QString id, QString datatype)
{
    addPort(name, id, true, 0, datatype);
}

//void DiagramItem::addInputPorts(const QStringList &names)
//{
//    foreach(QString n, names)
//        addInputPort(n);
//}

//void DiagramItem::addOutputPorts(const QStringList &names)
//{
//    foreach(QString n, names)
//        addOutputPort(n);
//}

// TODO 有问题
DiagramItem *DiagramItem::clone()
{
    qDebug() << "执行克隆操作";
    DiagramItem *b = new DiagramItem(nullptr);
    this->scene()->addItem(b);

    foreach(QGraphicsItem *port_, childItems())
    {
        if (port_->type() == ItemPort::Type)
        {
            ItemPort *port = (ItemPort*) port_;
            b->addPort(port->portName(), QUuid::createUuid().toString(),  port->isOutput(), port->portFlags(), port->getDataType());
        }
    }

    return b;
}

//QRectF DiagramItem::boundingRect()
//{
//    qreal adjust=0.5;
//    return QRectF(-50-adjust,-50-adjust,100+adjust*2,100+adjust*2);
//}

/**
 * @brief 刻画雷达组件
 * @param painter
 */
void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing, true);

    if(option->state & QStyle::State_Selected)
    {
        painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));
        // 不用重复画了
        // painter->drawRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));
    }
    else
    {
        painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        painter->setBrush(Qt::white);//白色填充
    }
    painter->drawPath(path());
}


/**
 * @brief 鼠标悬悬浮于组件上方事件
 * @param event
 */
void DiagramItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    qDebug() << "鼠标进入组件上方";
    //setCursor(Qt::OpenHandCursor);
    // 更新鼠标悬浮于组件上方样式
    setCursor(Qt::PointingHandCursor);
    event->accept();
}

void DiagramItem::removeArrow(Arrow *arrow)
{
//    int index = arrows.indexOf(arrow);

//    if (index != -1){
//        arrows.removeAt(index);
//    }
    qDebug() << this->getIconName() << "删除箭头（有可能是连线时的临时剪头）之前的arrows的大小" << arrows.size();
    arrows.removeOne(arrow);
    qDebug() << this->getIconName() << "删除箭头（有可能是连线时的临时剪头）之后的arrows的大小" << arrows.size();
}

void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows)
    {
        arrow->startItem()->removeArrow(arrow);
        arrow->port1()->removeConnection(arrow);
        arrow->port2()->removeConnection(arrow);
        arrow->endItem()->removeArrow(arrow);
        //RadarScene *sce = dynamic_cast<RadarScene*>(this->scene());
        //sce->idList.removeOne(arrow->itemId);
        scene()->removeItem(arrow);
        delete arrow;
    }
    arrows.clear();
}

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

void DiagramItem::addPort2mports(QString id, ItemPort *p)
{
    this->mports.insert(id, p);
}

QList<ItemPort *> DiagramItem::ports()
{
    QList<ItemPort*> res;
//    qDebug() << "diagram item的子items" << childItems() << endl<< "mports" << mports;
    foreach(QGraphicsItem *port_, childItems())
    {
        if (port_->type() == ItemPort::Type)
            res.append((ItemPort*) port_);
    }
    return res;
}

QString DiagramItem::getIconName() const
{
    return iconName;
}

void DiagramItem::setIconName(const QString &value)
{
    iconName = value;
}


//右键菜单，保持一致
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
//    qDebug() << "------------------" << this->iconName;
    if(this->iconName.compare("display") == 0)
    {
        qDebug() << "该组件是显示组件，禁用右键菜单中多余部分，只启用绘图（显示波形图）选项";
        // Antrn: WARNING 这里的8是我数出来的，以后菜单如果有变化，需要及时修改这个值
        // weishi: 恢复删除功能使用
        int index = 8;
        for(auto i : myContextMenu->actions())
        {
            if(myContextMenu->actions().indexOf(i) != index && myContextMenu->actions().indexOf(i) != 0)
            {
                i->setEnabled(false);
            }
        }


    }
    else
    {
        for(auto i : myContextMenu->actions())
            i->setEnabled(true);
    }

    scene()->clearSelection();
    setSelected(true);
    // 菜单可用
    myContextMenu->setEnabled(true);
    qDebug() << "myContextMenu设置为true--->" << myContextMenu->isEnabled();
    // 鼠标是不是在场景里
    bool is = dynamic_cast<RadarScene*>(this->scene())->isSelected;
    if(!is)
    {
        dynamic_cast<RadarScene*>(this->scene())->setIsSelected(true);
        qDebug() << "isSelected设置为true，场景被选中";
    };
    myContextMenu->exec(event->screenPos());
}

// This is an auto-generated comment.
/**
* @projectName   prototype_v0906
* @brief         简介 位置变化
* @author        Antrn
* @date          2019-10-07
*/
QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    //检测位置发生变化
    if (change == QGraphicsItem::ItemPositionChange)
    {
        if(this->init_pos_set != 0)
        {
//            qDebug() << "现在的位置： " << this->pos();
            // 获取最新位置，更新doc
            RadarScene *scene = dynamic_cast<RadarScene *>(this->scene());
            scene->updateXmlItemsPos(this->pos(), this);
        }
        else
        {
            // 初始化的时候，不用更新位置
            init_pos_set = 1;
        }
//        foreach (Arrow *arrow, arrows) {
//            arrow->updatePosition();
//        }
    }
    return value;
}

void DiagramItem::focusInEvent(QFocusEvent *)
{
//    qDebug() << "item focus in";
    bool is = dynamic_cast<RadarScene*>(this->scene())->isSelected;
    if(!is)
    {
        dynamic_cast<RadarScene*>(this->scene())->setIsSelected(true);
//        qDebug() << "isSelected设置为true";
    };

    //鼠标单击组件时，在队列信息中显示组件的相关信息
    comp_info comp;
    comp.comp_id = this->iconName;
    comp.cn_name = this->CN_Name;
    comp.uuid = this->itemSuuid;
    QVariant variant;
    variant.setValue(comp);
    //qDebug() <<"测试，当前组件的名称是" << comp.comp_id;

    emit show_comp_lineinfo(variant);

    myContextMenu->setEnabled(true);
}

/**
 * @brief 鼠标丢失焦点事件
 */
void DiagramItem::focusOutEvent(QFocusEvent *)
{
//    qDebug() << "item focus out";
    dynamic_cast<RadarScene*>(this->scene())->setIsSelected(false);
//    qDebug() << "isSelected设置为false";
    myContextMenu->setEnabled(false);
}

/**
 * @brief 鼠标双击动作触发展示右边属性
 * @param event
 */
void DiagramItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();
    setSelected(true);
    //确保左键点击.
    if (event->button() == Qt::LeftButton)
    {
        emit showItemsProperties(this->itemSuuid);
    }
    event->accept();
    QGraphicsPathItem::mousePressEvent(event);
}

int DiagramItem::getBufferSize() const
{
    return bufferSize;
}

void DiagramItem::setBufferSize(int value)
{
    bufferSize = value;
}

QString DiagramItem::getIn_whichIP() const
{
    return in_whichIP;
}

void DiagramItem::setIn_whichIP(const QString &value)
{
    in_whichIP = value;
}

int DiagramItem::getNum_thread() const
{
    return num_thread;
}

void DiagramItem::setNum_thread(int value)
{
    num_thread = value;
}

QMap<QString, ItemPort *> DiagramItem::getMports() const
{
    return mports;
}

void DiagramItem::setRadar_id(const QString &value)
{
    radar_id = value;
}

QString DiagramItem::getRadar_id() const
{
    return radar_id;
}

