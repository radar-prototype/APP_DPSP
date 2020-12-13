#ifndef DIAGRAMITEMPUBLIC_H
#define DIAGRAMITEMPUBLIC_H
#include <QMenu>
#include <QGraphicsPolygonItem>
#include <QObject>
#include <QDateTime>
#include "arrow.h"
//待理解--无关紧要
//class Arrow;
/**
 * @brief The DiagramItemPublic class  (目标：一个仿真对象（雷达，目标，对抗）的基类，没用到  用的是diagramitem)
 */
class DiagramItemPublic : public QGraphicsPolygonItem
{
public:

    DiagramItemPublic();

    enum { Type = UserType + 16 };

    //用来选择item类别
    enum DiagramType { Comp1, Comp2, Comp3, Comp4, Comp5};

    DiagramType diagramType() const {
        return myDiagramType;
    }

    //重写构造函数
    DiagramItemPublic(DiagramType diagramType, QMenu *contextMenu);

    void addArrow(Arrow *arrow);
    void removeArrow(Arrow *arrow);
    void removeArrows();

     int type() const override { return Type;}

    QPolygonF polygon() const { return myPolygon; }
    QPixmap image() const;

    //public??
    int itemId;
    QString iconName;
    int init_pos_set=0;


protected:
//    ??
//    WARN 暂时不能定义下面这几个，即使什么都不写也会有问题
//        void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
//    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    void KeyPressEvent(QKeyEvent *event);
//    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;


private:

    DiagramType myDiagramType;
    //自定义绘制多边形
    QPolygonF myPolygon;
    //保存的右键菜单，和菜单栏中的一致
    QMenu *myContextMenu;
    //存储箭头
    QList<Arrow *> arrows;

    QRectF boundingRect();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

};

#endif // DIAGRAMITEMPUBLIC_H
