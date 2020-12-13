#ifndef COMPONENT_LIST_LEFT_H
#define COMPONENT_LIST_LEFT_H
#include <QWidget>
#include <QListWidgetItem>
#include <QMap>
#include "algorithmcomp.h"
class Component_list_left : public QWidget
{
    Q_OBJECT
public:
    explicit Component_list_left(QWidget *parent = nullptr);
    //加载已有组建
    void loadAllComps(QWidget *parentWidget);
    QMap<QString, AlgorithmComp>* get_algorithms();
    QListWidget* get_listWidget();
    QList<QString>* get_nameList();
private:
    //    当前放组建的list
    QListWidget* listWidget;
    // id, ap
    QMap<QString, AlgorithmComp> algorithms;
    QList<QString> nameList;// 存放每个导入的组件的名字
    //    设置listwidget
    void newListWidget(QWidget *parentWidget);
    void addDragItem(QListWidgetItem *item);
    //    设置item
    QListWidgetItem  set_item(QString fileName);
signals:
    void clickItem(QListWidgetItem *item);

public slots:
    //    更新修改的组件
    void update_component(QString name_component);
   QList<QString> getNameList();


private slots:

};

#endif // COMPONENT_LIST_LEFT_H
