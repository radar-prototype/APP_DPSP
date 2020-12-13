#ifndef ECMITEM_H
#define ECMITEM_H
#include <QGraphicsItem>
#include <QMainWindow>
/**
* @projectName   prototype_v0719
* @brief         整体仿真中的对抗组件类。
* @author        zhengyuming
* @date          2019-08-20
*/
class ECMItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ECMItem();
    ~ECMItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);
    bool canDrag = false;

    QPointF start_click_point;
    QPointF release_click_point;

    QMainWindow* getMainWindow();

    QString getEcm_id() const;
    void setEcm_id(const QString &value);

public slots:
    void delete_item();
    void show_property();
    void edit_ecm();

signals:
    void close_mainwindow();

private:
    QString Ecm_id;


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

#endif // ECMITEM_H
