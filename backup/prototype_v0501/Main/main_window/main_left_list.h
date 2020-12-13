#ifndef MAIN_LEFT_LIST_H
#define MAIN_LEFT_LIST_H

#include <QWidget>
#include <QListWidget>
#include "menu_iteamoperation.h"
#include "page_structure/store.h"

class Main_left_list : public QListWidget
{
    Q_OBJECT
public:
    Main_left_list(QWidget *parent = nullptr);
    //添加item
    void add_listWidgetItem(QString  item_name);
public slots:
    //操作item
    void itemOperateSlot(Menu_iteamOperation::OperateType,  Page_structure_utils::window_type list_type,QString item_name,QString newName="");
    void set_list_type();
signals:
    void itemOperate(Menu_iteamOperation::OperateType, Page_structure_utils::window_type list_type, QString equipName,QString newName="");

protected:
    //开始拖拽
    void startDrag(Qt::DropActions supportedActions);

   //从该部件中拖拽出去的操作.
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private:
    //记录拖拽的起点.
    QPoint m_dragPoint;
    //记录被拖拽的项.
    QListWidgetItem *m_dragItem;

    static QString puzzleMimeType() {
        return QStringLiteral("image/x-module-piece");
    }

    //shao:修改listWidgetItem的相关逻辑，变成我比较喜欢的样子 代替原来写在utile里的函数
    //用来确定是三个list中的哪个
     Page_structure_utils::window_type list_type;
     QString equipName;
     //add按钮
     void listItem_add_button();
};

#endif // MAIN_LEFT_LIST_H
