#include "diagram_item.h"
#include <QDebug>
Diagram_item::Diagram_item(QMenu *contextMenu): DiagramItem(contextMenu)
{

}
void Diagram_item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
//    scene()->clearSelection();
//    setSelected(true);
    // 菜单可用
//    myContextMenu->setEnabled(true);
//    qDebug() << "myContextMenu设置为true--->" ;
    // 鼠标是不是在场景里
//    bool is = dynamic_cast<RadarScene*>(this->scene())->isSelected;
//    if(!is)
//    {
//        dynamic_cast<RadarScene*>(this->scene())->setIsSelected(true);
//        qDebug() << "isSelected设置为true，场景被选中";
//    };
//    myContextMenu->exec(event->screenPos());
}

