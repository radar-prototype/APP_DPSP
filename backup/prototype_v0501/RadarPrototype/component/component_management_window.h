#ifndef COMPONENT_MANAGEMENT_WINDOW_H
#define COMPONENT_MANAGEMENT_WINDOW_H
#include "algorithmcomp.h"
#include <QMainWindow>
#include "component_list_left.h"
#include "component_info.h"
#include "component_display_right.h"
#include "component_info_dialog.h"
namespace Ui {
class Component_management_window;
}

class Component_management_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Component_management_window(QWidget *parent = nullptr);
    ~Component_management_window();
//    //加载已有组建
//    void loadAllComps(QWidget *parentWidget);

private slots:
    void on_pushButton_add_clicked();

    void on_pushButton_import_clicked();

    void on_pushButton_change_clicked();

    void on_pushButton_delete_clicked();

private:
    //当前保存的
//     component_info_dialog* component_info_dialog;
     Component_list_left* component_list_left;
     Component_info* component_info;
     Component_display_right* component_display_right;
    AlgorithmComp algorithmComp; //存当前选中的组件
    Ui::Component_management_window *ui;

};

#endif // COMPONENT_MANAGEMENT_WINDOW_H
