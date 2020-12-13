#ifndef ENTRANCE_WIDGET_H
#define ENTRANCE_WIDGET_H

#include <QWidget>
#include "left_side_switch.h"
#include "hellodialog.h"
#include "mainwindow.h"
#include "mainwindownew.h"
#include "mainwindow_radar.h"
#include "mainwindow_ecm.h"
#include "component/component_management_window.h"
class Entrance_widget  : public QObject
{
public:
    Entrance_widget();
    void set_entrance();
    //按钮切换
    void switchover(int);

    Left_side_switch *getLeft_side_switch() const;

    HelloDialog *getHello() const;
    void setHello(HelloDialog *value);

    MainWindow *getMain_window() const;
    void setMain_window(MainWindow *value);

    MainWindowNew *getMain_window_new() const;
    void setMain_window_new(MainWindowNew *value);

    MainWindow_ECM *getMain_window_ecm() const;
    void setMain_window_ecm(MainWindow_ECM *value);

    MainWindow_Radar *getMain_window_radar() const;
    void setMain_window_radar(MainWindow_Radar *value);

    Component_management_window *getComponent_management_window() const;
private slots:
    void change_widget(int a,int b,QWidget *pWidget);
private:
    Left_side_switch* left_side_switch;
    HelloDialog* hello;
    MainWindow* main_window;
    MainWindowNew* main_window_new;
    MainWindow_ECM* main_window_ecm;
    MainWindow_Radar* main_window_radar;
    Component_management_window* component_management_window;
};

#endif // ENTRANCE_WIDGET_H
