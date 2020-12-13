#ifndef ARROW_H
#define ARROW_H

#include <QObject>
#include "diagramitem.h"
#include "arrowproperty.h"
#include <QGraphicsItem>
#include <QMenu>
#include <QAction>
#include <QVariant>
/**
* @projectName   prototype_v0719
* @brief         用于编辑雷达页面的组件元素之间链接的箭头类。
* @author        Antrn
* @date          2019-08-12
*/
//保存箭头的相关属性，可以传递给属性窗口
typedef struct Arrow_Data
{
    //起始端口和起始组件
    QString begin_port;
    QString begin_comp;
    //终止端口和终止组件
    QString end_port;
    QString end_comp;
    //组件ID
    QString ID;
}Arrow_Data;

Q_DECLARE_METATYPE(Arrow_Data)

class Arrow :public QObject,public QGraphicsPathItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 4 };
    int type() const override
    {
        return Type;
    }
    QString itemId;
    Arrow(DiagramItem *startItem, DiagramItem *endItem,
          QGraphicsItem *parent = nullptr);
    Arrow(QGraphicsItem *parent = nullptr);
//    ~Arrow() override;

    void setPos1(const QPointF &p);
    void setPos2(const QPointF &p);
    void setPort1(ItemPort *p);
    void setPort2(ItemPort *p);

    void updatePosFromPorts();
    void updatePath();
    ItemPort* port1() const;
    ItemPort* port2() const;


    // QRectF boundingRect() const override;
    // QPainterPath shape() const override;
    void setColor(const QColor &color)
    {
        myColor = color;
    }
    QString getColor()
    {
        return myColor.name();
    }
    DiagramItem *startItem() const
    {
        return myStartItem;
    }
    DiagramItem *endItem() const
    {
        return myEndItem;
    }
//    void updatePosition();

    void setMyStartItem(DiagramItem *value);

    void setMyEndItem(DiagramItem *value);
signals:
    //传递箭头的参数
    void send_ArrowInfo(QVariant);
    void mysignal();
    //删除箭头
    void  deletearrow_signal(QString id);

public slots:
    void delete_Arrow();
    void show_Arrow();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    // 起始终点在场景中的位置
    QPointF pos1;
    QPointF pos2;
    // 起始和终点的port
    ItemPort *m_port1;
    ItemPort *m_port2;

    DiagramItem *myStartItem;
    DiagramItem *myEndItem;
    QColor myColor;

    //右键点击出现菜单
    QMenu *Arrow_Menu;
    QAction *deleteAction;
    QAction *showAction;
    //属性窗口
    arrowproperty Arrow_property;

    //手动画出来的箭头
    // QPolygonF arrowHead;
};
#endif // ARROW_H
