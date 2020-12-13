#include "entrance_widget.h"
#include <QLabel>
#include <QIcon>
#include <QMovie>
#include "left_side_switch.h"
#include "unistd.h" //usleep()
#include "hellodialog.h"
#include "mysplashscreen.h"
#include "../../RadarPrototype/component/component_management_window.h"
#include "RadarPrototype/DeployManagement/clusterstatedialog.h"
//#include "RadarPrototype/DeployManagement/resourcemonitor.h"
#include <QDebug>
#include "deploymanagement.h"
Entrance_widget::Entrance_widget()
{
    this->left_side_switch = new class Left_side_switch();
    left_side_switch->setWindowTitle("数字化样机平台");
//        w->setWindowTitle("DIGITAL PROTOTYPE");
    left_side_switch->setWindowIcon(QIcon(":/img/fzkf.png"));
    this->hello = new class  HelloDialog();
    connect(this->hello, &HelloDialog::set_window, this, &Entrance_widget::change_widget);
}

void Entrance_widget::set_entrance()
{

    QToolButton *oneButton = left_side_switch->addTooButton("欢迎页", "欢迎页", QIcon(":/img/hi2.png"));
    left_side_switch->setToolWidget(oneButton, hello);

    QLabel* label4 = new QLabel("请在hello页面，选择要编辑的项目");// TODO 待修改ui
    left_side_switch->setToolWidget(left_side_switch->addTooButton("整体仿真", "整体仿真", QIcon(":/img/fz2.png")), label4);

    QLabel* label3 = new QLabel("还没有选择开发的设备");// TODO 待修改ui
    left_side_switch->setToolWidget(left_side_switch->addTooButton("开发板", "开发板", QIcon(":/img/kfb.png")), label3);

    this->component_management_window = new class Component_management_window();
    left_side_switch->setToolWidget(left_side_switch->addTooButton("组件库", "组件库", QIcon(":/img/zujian.png")), component_management_window);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("计算库", "计算库", QIcon(":/img/jsk.png")), hello);


    left_side_switch->setToolWidget(left_side_switch->addTooButton("日志管理", "日志管理", QIcon(":/img/logs.png")), hello);

    //部署管理界面
    //ClusterStateDialog *clusterStateDialog = new ClusterStateDialog();
    DeployManagement *deployManagement = new DeployManagement();
    left_side_switch->setToolWidget(left_side_switch->addTooButton("部署管理", "部署管理", QIcon(":/img/bs.png")), deployManagement);
#if 0
    // 这部分是启动页面
    QPixmap pixmap(":/img/startup.gif");
    MySplashScreen splash(pixmap, 1500);

    QLabel label(splash.w);
    QMovie mv(":/img/startup.gif");
    label.setMovie(&mv);
    mv.start();
    splash.show();
    splash.setCursor(Qt::BlankCursor);
//     left_side_switch->show();
//     oneButton->click();

    for (int i = 0; i < 100; i++)
    {
        qApp->processEvents();
        usleep(15000);
    }
    left_side_switch->show();
    splash.finish(left_side_switch);
#endif
    left_side_switch->show();
    oneButton->click();
//   this->switchover(1);
}

void Entrance_widget::switchover(int a)
{
    this->left_side_switch->switchover(a);
}
void Entrance_widget::change_widget(int a, int b, QWidget *pWidget)
{
    this->left_side_switch->change_widget(a, b, pWidget);
}
Left_side_switch *Entrance_widget::getLeft_side_switch() const
{
    return left_side_switch;
}

HelloDialog *Entrance_widget::getHello() const
{
    return hello;
}

void Entrance_widget::setHello(HelloDialog *value)
{
    hello = value;
}

MainWindow *Entrance_widget::getMain_window() const
{
    return main_window;
}

void Entrance_widget::setMain_window(MainWindow *value)
{
    main_window = value;
}

MainWindowNew *Entrance_widget::getMain_window_new() const
{
    return main_window_new;
}

void Entrance_widget::setMain_window_new(MainWindowNew *value)
{
    main_window_new = value;
}

MainWindow_ECM *Entrance_widget::getMain_window_ecm() const
{
    return main_window_ecm;
}

void Entrance_widget::setMain_window_ecm(MainWindow_ECM *value)
{
    main_window_ecm = value;
}

MainWindow_Radar *Entrance_widget::getMain_window_radar() const
{
    return main_window_radar;
}

void Entrance_widget::setMain_window_radar(MainWindow_Radar *value)
{
    main_window_radar = value;
}

Component_management_window *Entrance_widget::getComponent_management_window() const
{
    return component_management_window;
}

