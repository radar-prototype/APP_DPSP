#include "real_home_page.h"
#include "ui_real_home_page.h"
#include "./left_side_switch/left_utils.h"
#include "./left_side_switch/entrance_widget.h"

#include "unistd.h" //usleep()
#include "hellodialog.h"
#include "mysplashscreen.h"
#include "component/component_management_window.h"
#include "RadarPrototype/DeployManagement/clusterstatedialog.h"
#include "deploymanagement.h"
#include "store.h"
#include <QLabel>
#include <QIcon>
#include <QMovie>
#include <QDebug>
Real_home_page::Real_home_page(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Real_home_page)
{
    ui->setupUi(this);
    this->setWindowTitle("数字化样机平台");
    this->setWindowIcon(QIcon(":/img/fzkf.png"));
    this->whole_simulation_page=new Tab_page();
    this->development_board_page=new Tab_page();
    this->deploy_management_page=new Tab_page();
    this->hello = new class  HelloDialog();
    this->left_side_switch = new class Left_side_switch();
    this->component_management_window = new class Component_management_window();

    connect(this->whole_simulation_page,&Tab_page::tabCloseRequested,this,[](QWidget* page,bool* if_successful){
        MainWindow *pB = dynamic_cast<MainWindow*>(page);
//        *if_successful=Store::delet_main_window("", pB);
        *if_successful=Store::delet_window(Page_structure_utils::main,"", pB);
    });
    connect(this->development_board_page,&Tab_page::tabCloseRequested,this,[](QWidget* page,bool* if_successful){
        QString name=page->windowTitle();
        if(page->windowRole().toInt()==Page_structure_utils::radar){
            MainWindow_Radar *pB2 = dynamic_cast<MainWindow_Radar*>(page);
            *if_successful=Store::delet_window(Page_structure_utils::radar,"", pB2);
            qDebug()<<if_successful;
        }else if(page->windowRole().toInt()==Page_structure_utils::object){
            MainWindow_object *pB2 = dynamic_cast<MainWindow_object*>(page);
            *if_successful=Store::delet_window(Page_structure_utils::object,"", pB2);
        }else if(page->windowRole().toInt()==Page_structure_utils::ecm){
            MainWindow_ECM *pB2 = dynamic_cast<MainWindow_ECM*>(page);
            *if_successful=Store::delet_window(Page_structure_utils::ecm,"", pB2);
        }
    });
    connect(this->deploy_management_page,&Tab_page::tabCloseRequested,this,[](QWidget* page,bool* if_successful){

        *if_successful=Store::delet_window(Page_structure_utils::deploy,page->windowTitle());
    });

     this->set_left_side_switch();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(this->left_side_switch);
    this->centralWidget()->setLayout(layout);

    //shao:初始页面装配
    QLabel* label4 = new QLabel("请在hello页面，选择要编辑的项目");// TODO 待修改ui
    QLabel* label3 = new QLabel("还没有选择开发的设备");// TODO 待修改ui
    this->whole_simulation_page->add_page(label4,"说明页");
    this->development_board_page->add_page(label3,"说明页");
    //shao:删除默认添加的页
    this->whole_simulation_page->remove_page(-1);
    this->development_board_page->remove_page(-1);
    //ClusterStateDialog *clusterStateDialog = new ClusterStateDialog();
    DeployManagement *deployManagement = new DeployManagement();
    this->deploy_management_page->add_page(deployManagement,"部署第一页");
    this->deploy_management_page->remove_page(-1);
    //shao：所有的切换信号入口！！(信号传递路线：draglistwidget->HelloDialog /  MainWindow->DragListWidget->HelloDialog)
    connect(this->hello, &HelloDialog::set_window, this,[this](int a, int b, QWidget* widget){
        if(a==Left_utils::log||a==Left_utils::hello||a==Left_utils::component||b==Left_utils::only_switch){ //没有标签页这一层，直接
             this->getLeft_side_switch()->change_widget(a,b,widget);
        }else if(b==Left_utils::only_update||b==Left_utils::switch_update){
            if (a==Left_utils::whole_simulation){
                if(this->whole_simulation_page->index_of(widget)==-1){//还没有加入到里面（update）
                    this->whole_simulation_page->add_page(widget,widget->windowTitle());
                }else {
                    //shao：这样如果是更新（包含rename操作），才能修改标签name
                    this->whole_simulation_page->rename_page(widget,widget->windowTitle());
                    this->whole_simulation_page->swith_page(widget);
                }
                if(b==Left_utils::switch_update)
                //shao:因为是标签页，所以整体的大窗口是固定不切换的，实际在切换的是，大窗口里面的标签。
                 this->getLeft_side_switch()->change_widget(a,Left_utils::only_switch,nullptr);//（对应 switch）
            }else if(a==Left_utils::deploy_management){
                if(this->deploy_management_page->index_of(widget)==-1){//还没有加入到里面
                    this->deploy_management_page->add_page(widget,widget->windowTitle());
                }else {
                    this->deploy_management_page->rename_page(widget,widget->windowTitle());
                    this->deploy_management_page->swith_page(widget);
                }
                 if(b==Left_utils::switch_update)
                this->getLeft_side_switch()->change_widget(a,Left_utils::only_switch,nullptr);
            }else if(Left_utils::development_board){
                if(this->development_board_page->index_of(widget)==-1){//还没有加入到里面
                    this->development_board_page->add_page(widget,widget->windowTitle());
                }else {
                    this->development_board_page->rename_page(widget,widget->windowTitle());
                    this->development_board_page->swith_page(widget);
                }
                 if(b==Left_utils::switch_update)
                this->getLeft_side_switch()->change_widget(a,Left_utils::only_switch,nullptr);
            }
        }
    });
}

Real_home_page::~Real_home_page()
{
    delete ui;
}
void Real_home_page::set_left_side_switch()
{

    QToolButton *oneButton = left_side_switch->addTooButton("欢迎页", "欢迎页", QIcon(":/img/hi2.png"));
    left_side_switch->setToolWidget(oneButton, hello);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("整体仿真", "整体仿真", QIcon(":/img/fz2.png")), this->whole_simulation_page);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("开发板", "开发板", QIcon(":/img/kfb.png")), this->development_board_page);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("组件库", "组件库", QIcon(":/img/zujian.png")), component_management_window);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("计算库", "计算库", QIcon(":/img/jsk.png")), hello);

    left_side_switch->setToolWidget(left_side_switch->addTooButton("日志管理", "日志管理", QIcon(":/img/logs.png")), hello);

    //部署管理界面

    left_side_switch->setToolWidget(left_side_switch->addTooButton("部署管理", "部署管理", QIcon(":/img/bs.png")), this->deploy_management_page);
}

void Real_home_page::showWidget(){
    //shao:默认停留在hello页面
    this->show();
    this->left_side_switch->change_widget(Left_utils::hello,Left_utils::only_switch,nullptr);
    //shao:装配信号
    this->menu_signal_configuration(Left_utils::hello);

}
//集中处理信号，需要传入当前页面对象的指针，当前选中的左侧按钮是谁
void Real_home_page::menu_signal_configuration(int a){
    if(a==Left_utils::hello){

    }else if (a==Left_utils::log) {

    }else if (a==Left_utils::deploy_management) {

    }else if (a==Left_utils::component) {

    }else if (a==Left_utils::development_board) {

    }else if (a==Left_utils::whole_simulation) {

    }
    //2.跟当前页面相关的菜单项，在这里重新配置connect函数

    //3.页面变了之后，页面上的信号和菜单栏的槽函数连接

    //3.当左侧按钮切换的时候触发禁用某些菜单项

}

Component_management_window *Real_home_page::getComponent_management_window() const
{
    return component_management_window;
}

void Real_home_page::setComponent_management_window(Component_management_window *value)
{
    component_management_window = value;
}

Tab_page *Real_home_page::getDeploy_management_page() const
{
    return deploy_management_page;
}

void Real_home_page::setDeploy_management_page(Tab_page *value)
{
    deploy_management_page = value;
}

Tab_page *Real_home_page::getDevelopment_board_page() const
{
    return development_board_page;
}

void Real_home_page::setDevelopment_board_page(Tab_page *value)
{
    development_board_page = value;
}

Tab_page *Real_home_page::getWhole_simulation_page() const
{
    return whole_simulation_page;
}

void Real_home_page::setWhole_simulation_page(Tab_page *value)
{
    whole_simulation_page = value;
}

Left_side_switch *Real_home_page::getLeft_side_switch() const
{
    return left_side_switch;
}

void Real_home_page::setLeft_side_switch(Left_side_switch *value)
{
    left_side_switch = value;
}

void  Real_home_page::closeEvent(QCloseEvent *event){
    //没保存的窗口保存
    QMap<Page_structure_utils::window_type, QMap<QString ,QMainWindow*> >map=Store::getWindow_list();
    QMap<Page_structure_utils::window_type, QMap<QString ,QMainWindow*> >::const_iterator i=map.constBegin();
    while (i != map.constEnd()) {
        foreach(QMainWindow*  window,i.value()){
            if(!Store::delet_window(i.key(),"",window)){
                event->ignore() ;
                return;
            }
        }
        ++i;
    }
    event->accept();
}
