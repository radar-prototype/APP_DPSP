#ifndef STORE_H
#define STORE_H
#include <QObject>
#include <QMap>
#include <QString>
#include "mainwindow.h"
#include "mainwindow_radar.h"
#include "mainwindow_ecm.h"
#include "object_prototype/mainwindow_object.h"
#include "deploymanagement.h"
#include "page_structure_utils.h"

//shao:用来管理新建的各个页面类
class Store : public QObject
{
    Q_OBJECT
public:
    explicit Store(QObject *parent = nullptr);

    static QMap<Page_structure_utils::window_type,QMap<QString ,QMainWindow*>>   window_list;

        //添加
        static QMainWindow* add_window(Page_structure_utils::window_type window_type);
        //导入
        static QMainWindow* open_window(Page_structure_utils::window_type window_type,QString path);
        //修改项目名
        static QString edit_window_name(Page_structure_utils::window_type window_type,QString name);
        //删除
        static bool delet_window(Page_structure_utils::window_type window_type,QString name,QMainWindow* win=nullptr);

        static void update_prototype_map();

        static QMap<Page_structure_utils::window_type, QList<QString> > getExisting_prototype_name();




        static QMap<Page_structure_utils::window_type, QMap<QString, QMainWindow *> > getWindow_list();

signals:

public slots:

private:
        enum exist_type
        {
            inexistence=0,exist_not_transition,exist_transition
        };


        //判断部分的公共代码提出
    static int window_if_exist(Page_structure_utils::window_type window_type,QString name);

    static QMap<Page_structure_utils::window_type ,QList<QString>> existing_prototype_name;


};

#endif // STORE_H
