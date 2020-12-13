#include "ecmitem.h"
#include "mainwindow_ecm.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include "ecmproperty.h"

/**
* @projectName   prototype_v0719
* @brief         整体仿真中的对抗组件类。
* @author        zhengyuming
* @date          2019-08-20
*/

ECMItem::ECMItem()
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemIsMovable);
}

ECMItem::~ECMItem()
{

}

QMainWindow* ECMItem::getMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets())
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w))
            return mainWin;
    return nullptr;
}


QRectF ECMItem::boundingRect() const
{
    qreal adjust=0.5;
    return QRectF(0-adjust,0-adjust,70+adjust,70+adjust);
}

void ECMItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawRect(0,0,70,70);
    painter->drawPixmap(11,11,48,48, QPixmap(":/img/ECM.png"));
}

void ECMItem::delete_item()
{
    qDebug()<<"删除对抗"<<endl;
    this->destroyed();
}

void ECMItem::show_property()
{
    qDebug() << "show Property";
    EcmProperty *ep = new EcmProperty();
    ep->exec();
}

void ECMItem::edit_ecm()
{
    MainWindow_ECM *main_ECM = new MainWindow_ECM(getEcm_id());
    //connect(this, &RadarItem::close_mainwindow, getMainWindow(), &QMainWindow::hide);
    //emit close_mainwindow();
    main_ECM->show();
}

QString ECMItem::getEcm_id() const
{
    return Ecm_id;
}

void ECMItem::setEcm_id(const QString &value)
{
    Ecm_id = value;
}

void ECMItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "鼠标点击对抗了";
    setFocus();
    setSelected(true);
    setCursor(Qt::ClosedHandCursor);
    event->accept();
    QGraphicsItem::mousePressEvent(event);
}

void ECMItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    show_property();
    QGraphicsItem::mouseDoubleClickEvent (event );
}

void ECMItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu *menu = new QMenu;
    menu->addAction(tr("删除"), this, SLOT(delete_item()));
    // FIXME 此处和雷达共用属性窗口
    menu->addAction(tr("属性"), this, SLOT(show_property()));
    // FIXME 此处和雷达共用编辑窗口
    menu->addAction(tr("编辑"), this, SLOT(edit_ecm()));
    menu->exec(event->screenPos());
    delete menu;
}
