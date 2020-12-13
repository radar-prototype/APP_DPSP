#ifndef RADARSCENE_H
#define RADARSCENE_H

/**
* @projectName   prototype_v0719
* @brief         编辑雷达页面的自定义场景类，上面绘制设计雷达的组件链接。
* @author        Antrn
* @date          2019-08-12
*/
#include "diagramitem.h"
#include "diagramtextitem.h"

#include <QtXml>
#include <QGraphicsScene>
#include <QMenu>
#include <QObject>
#include <algorithmcomp.h>
#include <QRubberBand>

class RadarScene : public QGraphicsScene
{
    Q_OBJECT

public:
    // 整理好顺序之后的场景中算法组件的唯一id，name+sid，当有并行组件的时候，这个不能用，因为即使index相同，插入时也会有先后顺序
    QList<QString> orderCompSidList;
    // index 相同算法组件name+id的放一起，每个元素的下标加一就是里面组件的index
    QList<QList<QString>> diagramListByindex;

    bool isSelected = false;
    void setIsSelected(bool b)
    {
        isSelected = b;
    }
    //InsertItem默认
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem };

    explicit RadarScene(QMenu *itemMenu, QObject *parent = nullptr);

    QFont font() const
    {
        return myFont;
    }
    QColor textColor() const
    {
        return myTextColor;
    }
    QColor itemColor() const
    {
        return myItemColor;
    }
    QColor lineColor() const
    {
        return myLineColor;
    }
//    QGraphicsItem *itemAt(const QPointF &pos);

    void setLineColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setItemColor(const QColor &color);
    void setFont(const QFont &font);

    void modifyXmlItems(QPointF pos, DiagramItem *item);
    void updateXmlItemsPos(QPointF pos, DiagramItem *item);
    void modifyXmlArrows(Arrow *arrow, DiagramItem *startItem, DiagramItem *endItem);

    QMenu *getItemMenu()
    {
        return myItemMenu;
    }

    QDomDocument *getDoc()
    {
        return &doc;
    }

    // 场景中所有itmem的id
    QList<QString> idList;

    QString getMyItemIconName() const;

    void setDoc(const QDomDocument &value);

    QDomElement *getArrs();
    void setArrs(const QDomElement &value);

    QDomElement *getItems();
    void setItems(const QDomElement &value);

    QColor getMyItemColor() const;
    void setMyItemColor(const QColor &value);

    QMap<QString, AlgorithmComp> getScene_comps() const;
    void setScene_comps(const QMap<QString, AlgorithmComp> &value);
    void add2Scene_comps(QString key, AlgorithmComp &a);
    void deleteScene_comps(QString id);

    // 向子空间加入文件
    void createFile2zoom(QString sid);

    // algoXml
    void getPara2Port(QString fileName, DiagramItem *item);

    int getCompNums() const;
    void setCompNums(int value);

    bool getIsExistInput() const;
    void setIsExistInput(bool value);
    void setIsExistOutput(bool value);

    bool getIsExistDisplay() const;
    void setIsExistDisplay(bool value);

public slots:
    void setMode(Mode mode);
    //    void setItemType(DiagramItem::DiagramType type);
    void setMyItemIconName(QString value);
    void editorLostFocus(DiagramTextItem *item);
    void startRunCode();
    void sendRate(float rate);
    //接受箭头的删除信号
    void deal_arrowdelte(QString id);


    // 接收来自radarcompdraglistwidget的算法信息
    void receiveAlgo4listWidget(AlgorithmComp ap);

signals:
    void signal_xy(double x, double y);

    void itemInserted(DiagramItem *item);
    void textInserted(QGraphicsTextItem *item);
    void itemSelected(QGraphicsItem *item);

    void startRun();
    void rateSignal(float rate);
    void overRun();
    //发出箭头的删除信号
    void delete_arrowSignal(QString id);

    // 通知MainWindow_Radar xml已经改变
    void isSave2False(QString message);

    void refreshPropertyDock(AlgorithmComp ac, bool isReadOnly);


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
//    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    // 支持拖拽
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    // 橡皮筋
    QRubberBand *rubberBand;
    QPointF origin;
    bool isDrag = false;

    // 当分发代码时，记录组件的数量
    int CompNums = 0;

    bool isItemChange(int type);

//    DiagramItem::DiagramType myItemType;
    QString myItemIconName;
    DiagramTextItem *textItem;
    // 菜单
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    // 旧版直线
    //QGraphicsLineItem *line;
    // 新版曲线段
    Arrow *conn;

    QFont myFont;
    QColor myTextColor;
    QColor myItemColor;
    QColor myLineColor;

    // 存储场景数据
    QDomDocument doc;
    QDomElement Arrs;
    QDomElement Items;

    // mao: 场景中所有的算法存在列表中
    QMap<QString, AlgorithmComp> scene_comps;
    // mao: 临时存放listWidget在点击和拖动时传过来的对象
    AlgorithmComp ap;
    // mao: 场景中是否已经有输入组件以及输出组件
    bool isExistInput = false;
    bool isExistDisplay = false;
    bool isExistOutput = false;
};

#endif // RADARSCENE_H
