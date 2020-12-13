#ifndef ITEMPORT_H
#define ITEMPORT_H

#include <QGraphicsPathItem>

class DiagramItem;
class Arrow;
/**
 * @brief The ItemPort class  组件端口
 */
class ItemPort : public QGraphicsPathItem
{
public:
//    ItemPort();
    ItemPort(QString name, bool isOutput, QGraphicsItem *parent = nullptr);

    enum { Type = QGraphicsItem::UserType + 1 };
    // 0,1,2,3分别是输入输出端口，组件的名字，类型名(中文名)，参数名
    enum { IOPort = 0, NamePort = 1, TypePort = 2, ParamPort = 3};
//    ~ItemPort();

    void setItemBlock(DiagramItem*);
//    void setName(const QString &n);
//    void setIsOutput(bool o);
    int radius();
    bool isOutput();
//    QList<Arrow *> &connections();
    void addArrow2Connections(Arrow * a);
    // 设置
    void setPortFlags(int);

    const QString& portName() const
    {
        return name;
    }
    int portFlags() const
    {
        return m_portFlags;
    }

    int type() const
    {
        return Type;
    }

    DiagramItem* block() const;

    bool isConnected(ItemPort*);

    void removeConnection(Arrow * arrow);

    QString getUuid() const;
    void setUuid(const QString &value);

    QList<Arrow *> getConnections() const;

    QString getDataType() const;
    void setDataType(const QString &value);

    bool getIsOutput() const;
    void setIsOutput(bool isOutput);

    int getPortFlags() const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    QString uuid;
    // 自己所在的block
    DiagramItem *m_block;
    // 组件的名字
    QString name;
    // 是不是输出端口
    bool isOutput_;
    QGraphicsTextItem *label;
    int radius_;
    int margin;
    QList<Arrow*> m_connections;
    // 端口号的flag，0输入，1输出，2其他
    int m_portFlags;

    // 数据类型
    QString dataType = "";
};

#endif // ITEMPORT_H
