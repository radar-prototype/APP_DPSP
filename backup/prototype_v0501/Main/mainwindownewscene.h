#ifndef MAINWINDOWNEWSCENE_H
#define MAINWINDOWNEWSCENE_H
/**
* @projectName   prototype_v0719
* @brief         新建工程的主场景，用户配置雷达或者电子对抗，用户整体仿真。
* @author        Antrn
* @date          2019-08-12
*/
#include <QGraphicsScene>
#include <QPaintEvent>
#include <QGraphicsItem>
#include <QDrag>
#include "ecmitem.h"
#include "radaritem.h"
#include "arrowitem.h"
//为了可以识别枚举值 OperateType
#include "menu_iteamoperation.h"
#include "page_structure/page_structure_utils.h"
//#include "page_structure/store.h"
/**
 * @brief The MainWindowNewScene class 总体仿真窗口的主要显示控件
 */
class MainWindowNewScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MainWindowNewScene();
    QList<QGraphicsItem*> Item_List;
    QPointF computeFrom(const QRectF& from, const QPointF& to);
    void computeLine(QGraphicsItem*from, QGraphicsItem*to);
    QLineF computeMinLength(QList<QLineF> line_list);
    bool dragFlag;

    QList<QGraphicsItem*> connect_to_List;
    QList<QGraphicsItem*> connect_from_List;
    QList<ArrowItem*> delete_line_List;
    QList<ArrowItem*> graphicsLineItem_List;

    void setlineBeginItem(QGraphicsItem *value);

public slots:
    void itemOperateSlot(Menu_iteamOperation::OperateType, Page_structure_utils::window_type list_type, QString item_name, QString newName = "");

private slots:
    void LinePaint();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    //void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void paintEvent(QPaintEvent *);
    void drawBackground(QPainter*, const QRectF &rect);

signals:
    void signal_xy(double x, double y);
    void rateSignal(float rate);
    void showParamSettingBtn(bool tof);
    //接收来至于子类的item操作(操作,设备类型，itemId)
    void itemOperate(Menu_iteamOperation::OperateType, Page_structure_utils::window_type list_type, QString id);

private:
    // 拖动时候的起点
//    QPointF startPoint;
    //临时保存起点终点
    QPointF sourcePoint;
    QPointF destPoint;
    QGraphicsItem *line_begin_item;
    QGraphicsItem *drag_item;
    // 保存拖入场景的雷达item（shao:如果定义了对抗，目标，雷达的一个公共的，父组件就好了）
    // Antrn:有了，满足你
    //shao:????；聊起来了
    QMap<QString, QGraphicsItem*> id_gItem;
};

#endif // MAINWINDOWNEWSCENE_H
