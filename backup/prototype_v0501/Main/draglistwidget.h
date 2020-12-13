#ifndef DRAGLISTWIDGET_H
#define DRAGLISTWIDGET_H
#include <QWidget>
#include <QtGui>
#include <QListWidget>
#include <QApplication>
#include "menu_iteamoperation.h"
#include<QDialog>
#include<QMap>
#include "mainwindow_radar.h"
//动态命名弹出窗口
#define SET_RADARNAME(name) radar##name
/**
* @projectName   prototype_v0719
* @brief         整体工程项目左边的拖拽列表：雷达/对抗/目标，是以list的形式表现的。
* @author        Antrn
* @date          2019-08-12
*/
class DragListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit DragListWidget(QWidget *parent = nullptr);
    // 没用
    void addDragItem(QListWidgetItem*item);
    // 往id_item中插入
    void insert2id_item(QString, QListWidgetItem*);

    bool closeDragListWidget();

    static QString puzzleMimeType() {
        return QStringLiteral("image/x-module-piece");
    }

    // 添加item，雷达/对抗/目标
    void listItem_add(QString);

    QMap<QString, QListWidgetItem *> getId_item() const;

    QSet<QString> getNameSet() const;

    QSet<QString> nameSet;
    void add2NameSet(QString);

    QString getRadarType() const;

    QString getEcmType() const;

    QString getTarType() const;

signals:
    void itemOperate(Menu_iteamOperation::OperateType, QString equipType, QString equipName, QString newName="");
    void set_window(int a,int b, QWidget* Widget);//触发左边按钮切换窗口
protected:
    //开始拖拽
    void startDrag(Qt::DropActions supportedActions);

   //从该部件中拖拽出去的操作.
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

//    void mouseMoveEvent(QMouseEvent *event);
//    //接受外来拖拽的操作.
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dropEvent(QDropEvent *event);

private:
    //记录拖拽的起点.
    QPoint m_dragPoint;
    //记录被拖拽的项.
    QListWidgetItem *m_dragItem;
    // 老代码
    // QListWidgetItem *addRadarButton;

    // key QString:name
    QMap<QString, QListWidgetItem*> id_item;

    //listWidgetItem计数
    int id_inde=1;

    //是否新建了edit窗口
    QMap<QString,MainWindow_Radar*> newEditWindowList;
    //保存前一个被操作的item信息（名字 index）
    struct forRename
    {
        QString preName;
        int preIndex=-1;
        bool ifEnableChange=true;
    }forRename;

    //今后传值方便
    QString radarType="雷达", ecmType="对抗", tarType="目标";


public slots:
    //操作item
    void itemOperateSlot(Menu_iteamOperation::OperateType, QString ,QString newName="");
    void renameSlot(QListWidgetItem* );
};

#endif // DRAGLISTWIDGET_H
