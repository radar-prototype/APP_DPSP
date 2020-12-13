#ifndef DIAGRAMTEXTITEM_H
#define DIAGRAMTEXTITEM_H

#include <QObject>
#include <QGraphicsItem>
/**
* @projectName   prototype_v0719
* @brief         这个是文字图表类，用于在雷达组件上编辑文字或备注等信息。（是一个注释组件  现在没用到了）
* @author        Antrn
* @date          2019-08-12
*/
class DiagramTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 17 };

    DiagramTextItem(QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

signals:
    void lostFocus(DiagramTextItem *item);
    void selectedChange(QGraphicsItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // DIAGRAMTEXTITEM_H
