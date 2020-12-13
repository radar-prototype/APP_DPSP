#ifndef REAL_HOME_PAGE_H
#define REAL_HOME_PAGE_H

#include <QMainWindow>
#include "./left_side_switch/left_side_switch.h"
#include "hellodialog.h"
#include "mainwindow.h"
#include "mainwindownew.h"
#include "mainwindow_radar.h"
#include "mainwindow_ecm.h"
#include "component/component_management_window.h"
#include "tab_page.h"
namespace Ui {
class Real_home_page;
}

class Real_home_page : public QMainWindow
{
    Q_OBJECT

public:
    explicit Real_home_page(QWidget *parent = 0);
    ~Real_home_page();
    void showWidget();
    void menu_signal_configuration(int a);

    Component_management_window *getComponent_management_window() const;
    void setComponent_management_window(Component_management_window *value);

    Tab_page *getDeploy_management_page() const;
    void setDeploy_management_page(Tab_page *value);

    Tab_page *getDevelopment_board_page() const;
    void setDevelopment_board_page(Tab_page *value);

    Tab_page *getWhole_simulation_page() const;
    void setWhole_simulation_page(Tab_page *value);

    Left_side_switch *getLeft_side_switch() const;
    void setLeft_side_switch(Left_side_switch *value);

private:
    Ui::Real_home_page *ui;
    Left_side_switch* left_side_switch;
    Tab_page* whole_simulation_page;
    Tab_page* development_board_page;
    Tab_page* deploy_management_page;
    HelloDialog* hello;
    Component_management_window* component_management_window;

    void set_left_side_switch();

    virtual void  closeEvent(QCloseEvent *event);

};

#endif // REAL_HOME_PAGE_H
