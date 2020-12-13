#ifndef DIAGRAM_ITEM_H
#define DIAGRAM_ITEM_H
#include "RadarPrototype/diagramitem.h"
/**
 * @brief The Diagram_item class 为了解决预览右击奔溃 隐藏的方式复写右击事件
 */
class Diagram_item : public DiagramItem
{
public:
    Diagram_item(QMenu *contextMenu);
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};
#endif // DIAGRAM_ITEM_H
