#include "diagramitempublic.h"
#include <QPainter>
#include <QDebug>
#include "graphicsscenepublic.h"
DiagramItemPublic::DiagramItemPublic(DiagramType diagramType, QMenu *contextMenu)
{
    myDiagramType = diagramType;

    myContextMenu = contextMenu;

    QPainterPath path;

    //用来控制item类型的 暂时用不到
    {
//          //这里元素的大小和在场景中是一样的，修改了都会变。
//    switch (myDiagramType) {
//        //暂不使用
//        case Comp3:
////            path.moveTo(200, 50);
////            path.arcTo(150, 0, 50, 50, 0, 90);
////            path.arcTo(50, 0, 50, 50, 90, 90);
////            path.arcTo(50, 50, 50, 50, 180, 90);
////            path.arcTo(150, 50, 50, 50, 270, 90);
////            path.lineTo(200, 25);
////            myPolygon = path.toFillPolygon();
//        //统一形状
//            myPolygon << QPointF(-50, -50) << QPointF(50, -50)
//                      << QPointF(50, 50) << QPointF(-50, 50)
//                      << QPointF(-50, -50);
//            break;
//        //立着正方形
//        case Comp2:
////            myPolygon << QPointF(-50, 0) << QPointF(0, 50)
////                      << QPointF(50, 0) << QPointF(0, -50)
////                      << QPointF(-50, 0);

//        //统一形状
//            myPolygon << QPointF(-50, -50) << QPointF(50, -50)
//                      << QPointF(50, 50) << QPointF(-50, 50)
//                      << QPointF(-50, -50);
//            break;
//        //正方形
//        case Comp1:
////            path.moveTo(-50, -50);
////            path.addText(QPointF(0,0),QFont("Helvetica", 20),QString("hhh"));
////            path.moveTo(50, -50);
////            path.lineTo(50, -50);
////            path.lineTo(50, 50);
////            path.lineTo(-50, 50);
////            path.lineTo(-50, -50);

//        //统一形状
//            myPolygon << QPointF(-50, -50) << QPointF(50, -50)
//                      << QPointF(50, 50) << QPointF(-50, 50)
//                      << QPointF(-50, -50);
////            myPolygon = path.toFillPolygon();
//            break;
//        //默认 菱形，IO
//        default:
////            myPolygon << QPointF(-60, -40) << QPointF(-35, 40)
////                      << QPointF(60, 40) << QPointF(35, -40)
////                      << QPointF(-60, -40);
//        //统一形状
//            myPolygon << QPointF(-50, -50) << QPointF(50, -50)
//                      << QPointF(50, 50) << QPointF(-50, 50)
//                      << QPointF(-50, -50);
//            break;
//    }
    }
    //设置item为矩形
    myPolygon << QPointF(-50, -50) << QPointF(50, -50)
              << QPointF(50, 50) << QPointF(-50, 50)
              << QPointF(-50, -50);

    setPolygon(myPolygon);
    //属性设置
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    //坐标变化
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);
}

QRectF DiagramItemPublic::boundingRect()
//修改位置？
{
    qreal adjust = 0.5;
    return QRectF(-50 - adjust, 50 - adjust, 100 + adjust, 100 + adjust);
}

void DiagramItemPublic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawRect(-50, -50, 100, 100);
    myDiagramType = diagramType();
    switch (myDiagramType)
    {
    case Comp1:
        painter->drawPixmap(-49, -49, 98, 98, QPixmap(":/img/FDPC.png"));;
        break;
    case Comp2:
        painter->drawPixmap(-49, -49, 98, 98, QPixmap(":/img/CFAR.png"));;
        break;
    case Comp4:
        painter->drawPixmap(-49, -49, 98, 98, QPixmap(":/img/MTD.png"));;
        break;
    case Comp3:
        painter->drawPixmap(-49, -49, 98, 98, QPixmap(":/img/input.png"));;
        break;
    case Comp5:
        painter->drawPixmap(-49, -49, 98, 98, QPixmap(":/img/output.png"));;
        break;
    }
}

void DiagramItemPublic::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

void DiagramItemPublic::removeArrows()
{
//    foreach (Arrow *arrow, arrows) {
//        arrow->startItem()->removeArrow(arrow);
//        arrow->endItem()->removeArrow(arrow);
//        //GraphicsScenePublic *sce = dynamic_cast<GraphicsScenePublic*>(this->scene());
//        //sce->idList.removeOne(arrow->itemId);
//        scene()->removeItem(arrow);
//        delete arrow;
//    }
}

void DiagramItemPublic::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

QPixmap DiagramItemPublic::image() const
{
    QPixmap pixmap(250, 250);
//    pixmap.fill(Qt::transparent);
    QString iconName;
    switch (diagramType())
    {
    case DiagramItemPublic::DiagramType::Comp1 :
        iconName = "FDPC";
        break;
    case DiagramItemPublic::DiagramType::Comp2 :
        iconName = "CFAR";
        break;
    case DiagramItemPublic::DiagramType::Comp4 :
        iconName = "MTD";
        break;
    case DiagramItemPublic::DiagramType::Comp3 :
        iconName = "INPUT";
        break;
    case DiagramItemPublic::DiagramType::Comp5 :
        iconName = "OUTPUT";
        break;
    }
    QString itemIcon = ":/img/" + iconName + ".ico";
    pixmap.convertFromImage(QImage(itemIcon));
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
//    painter.drawPolyline(myPolygon);
    painter.setRenderHint(QPainter::Antialiasing, true);
    return pixmap;
}

//右键菜单，保持一致
void DiagramItemPublic::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
//    scene()->clearSelection();
//    setSelected(true);
//    myContextMenu->exec(event->screenPos());
}
//移动即触发
QVariant DiagramItemPublic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    //检测位置发生变化
    if (change == QGraphicsItem::ItemPositionChange)
    {
//        qDebug()<<"itemChange";
        if(this->init_pos_set != 0)
        {
//            qDebug() << "现在的位置： " << this->pos();
            // 获取最新位置，更新doc
//            GraphicsScenePublic *scene = dynamic_cast<GraphicsScenePublic *>(this->scene());
//            scene->updateXmlItemsPos(this->pos(), this);
        }
        else
        {
            init_pos_set = 1;
        }
//        foreach (Arrow *arrow, arrows) {
//            arrow->updatePosition();
//        }
    }
//    update();
    return value;
}
