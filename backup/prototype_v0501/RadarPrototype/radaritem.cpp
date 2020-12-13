#include "mainwindow_radar.h"
#include "radaritem.h"
#include "radarproperty.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <mainwindownew.h>
/**
* @projectName   prototype_v0719
* @brief         整体仿真中的雷达组件类。
* @author        Antrn
* @date          2019-08-12
*/
RadarItem::RadarItem(QString id): QGraphicsItem()
{
    setFlags(QGraphicsItem::ItemIsFocusable|QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    this->setRadar_id(id);
}

RadarItem::~RadarItem()
{

}

QMainWindow* RadarItem::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return mainWin;
    return nullptr;
}


QRectF RadarItem::boundingRect() const
{
    qreal adjust=0.5;
    return QRectF(0-adjust,0-adjust,70+adjust,70+adjust);
}

void RadarItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->drawPixmap(11,11,48,48, QPixmap(":/img/radar.png"));

    if(option->state & QStyle::State_Selected){
        painter->setPen(QPen(Qt::darkGray, 1, Qt::DashLine, Qt::SquareCap,Qt::MiterJoin));
        painter->drawRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));
    }else{
        painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::SquareCap,Qt::MiterJoin));
        painter->drawRect(0,0,70,70);
    }
}

void RadarItem::delete_item()
{
    this->destroyed();
}

void RadarItem::show_property()
{
    qDebug() << "show Property";
    RadarProperty *p = new RadarProperty();
    p->exec();
}

#if 0
// 设置使得在雷达页面未关闭之前不能关闭主窗口。
// 使得每个新打开的页面都和唯一一个雷达/对抗绑定，如果对于某个雷达已经配置好，下次编辑直接可以看到雷达里面的组件（场景）
void RadarItem::edit_radar()
{
    MainWindow_Radar *main_radar = new MainWindow_Radar(getRadar_id());
    MainWindowNew::main_radar_list.append(main_radar);
    main_radar->show();
}
#endif

QString RadarItem::getRadar_id(){
    return radar_id;
}

void RadarItem::setRadar_id(QString value)
{
    radar_id = value;
    this->setToolTip("组件名:"+value);
}

#if 0
void RadarItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "鼠标点击雷达了";
    setFocus();
    setSelected(true);
    setCursor(Qt::ClosedHandCursor);
    event->accept();
    QGraphicsItem::mousePressEvent(event);
}

void RadarItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "鼠标松开雷达了";
    QGraphicsItem::mouseReleaseEvent(event);
}
#endif
void RadarItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit itemOperate(Menu_iteamOperation::edit,radar_id);
    QGraphicsItem::mouseDoubleClickEvent (event );
}

void RadarItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
        Menu_iteamOperation *menu = new Menu_iteamOperation();

        connect(menu , &Menu_iteamOperation::itemOperate,[=](Menu_iteamOperation::OperateType operate){
            //信号---->信号（向父容器传递）
            if(operate==Menu_iteamOperation::del){
                //删除不用传
                this->destroyed();
            }else {
                emit itemOperate(operate,this->radar_id);
            }

        });
        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->exec(event->screenPos());
        delete menu;
}

void RadarItem::itemOperateSlot(Menu_iteamOperation::OperateType operateType,QString newName){
    switch (operateType){
    case Menu_iteamOperation::del:
        qDebug()<<"item_delete:";
        this->destroyed();
        break;
    case Menu_iteamOperation::edit:
         qDebug()<<"item_edit:";
        break;
    case Menu_iteamOperation::property:
        qDebug()<<"item_property:";
        break;
    case Menu_iteamOperation::rename:
        qDebug()<<"item_rename:";
        this->setRadar_id(newName);
        break;
    case Menu_iteamOperation::ppi:
        qDebug()<<"item_ppi:";
        break;
    case Menu_iteamOperation::ashow:
        qDebug()<<"item_ashow:";
        break;
    }
}
