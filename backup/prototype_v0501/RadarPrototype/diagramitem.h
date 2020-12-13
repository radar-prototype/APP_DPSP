#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H
#include <QMetaEnum>
#include <QDateTime>
#include <QGraphicsItem>
#include <QObject>
#include <compproperty.h>
#include "itemport.h"
#include <QVariant>

class Arrow;

/**
* @projectName   prototype_v0719
* @brief         这里就是悬浮框中雷达内部组件的图表类。（组件）
* @author        Antrn
* @date          2019-08-12
*/

class DiagramItem : public QObject, public QGraphicsPathItem
{
    Q_OBJECT
public:
    int index = -1; // 排序时的重要索引
    // 算法组件的标识  Type是官方提供的一个QGraphicsPathItem的标识
    //实现的时候自己随机加一个数字来区分不同的QGraphicsPathItem的实现类（例如：我们区分 端口 和 算法组件）
    enum { Type = UserType + 15 };
    DiagramItem(QMenu *contextMenu, QGraphicsItem *parent = nullptr);
    DiagramItem(QString itemUuid, QString iconName, QMenu *contextMenu, QString radar_id, QGraphicsItem *parent = nullptr);
    ItemPort* addPort(const QString &name, QString id, bool isOutput, int flags = 0, QString dt = "");

    void addInputPort(const QString &name, QString id, QString datatype);
    void addOutputPort(const QString &name, QString id, QString datatype);
//    void addInputPorts(const QStringList &names);
//    void addOutputPorts(const QStringList &names);

    DiagramItem* clone();
    // 属性窗口
    CompProperty *comp;

    void removeArrow(Arrow *arrow);
    void removeArrows();
    // QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    void addPort2mports(QString id, ItemPort *p);

    // 端口对象
    QList<ItemPort*> ports();
    int type() const override
    {
        return Type;
    }
    // 一开始的方案：使用int唯一id
    // int itemId;
    // 后来使用uuid代替int形式
    QString itemSuuid;
    // Antrn: 组件小图标，后来被作为组件名
    QString iconName;
    // 中文名
    QString CN_Name;
    // 是否初始化的标志
    int init_pos_set = 0;

    // 函数调用
    QString callFunction;

    // QRectF boundingRect();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QString getIconName() const;
    void setIconName(const QString &value);

    QString getRadar_id() const;

    void setRadar_id(const QString &value);

    QMap<QString, ItemPort *> getMports() const;

    int getNum_thread() const;
    void setNum_thread(int value);

    QString getIn_whichIP() const;
    void setIn_whichIP(const QString &value);

    int getBufferSize() const;
    void setBufferSize(int value);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void showItemsProperties(QString sid);
    //展示组件的队列信息
    void show_comp_lineinfo(QVariant variant);

private:
    //自定义绘制多边形
    // QPolygonF myPolygon;
    //保存的右键菜单，和菜单栏中的一致
    QMenu *myContextMenu;
    // 存储箭头
    QList<Arrow *> arrows;
    // 记住这个组件是属于哪个雷达的
    QString radar_id;
    // 键为id，值为端口指针
    QMap<QString, ItemPort*> mports;
    // 水平边距
    int horzMargin;
    int vertMargin;
    // 宽高
    int width;
    int height;

    // Antrn: 线程数
    int num_thread = 1;
    // Antrn: 分配在哪台节点,ip
    QString in_whichIP = "127.0.0.1";
    // Antrn: 组件数据的大小，便于显示组件获取分配内存
    int bufferSize = 82336;
};

//记录组件的相关信息,后续可以添加其他组件的属性，目前用于传递到队列信息的表格中进行显示
typedef struct comp_info
{
    //组件ID
    QString comp_id;
    //组件中文名
    QString cn_name;
    //
    QString uuid;
} comp_info;

Q_DECLARE_METATYPE(comp_info)
#endif // DIAGRAMITEM_H
