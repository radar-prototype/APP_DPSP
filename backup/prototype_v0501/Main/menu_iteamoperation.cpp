#include "menu_iteamoperation.h"
#include "ui_menu_iteamoperation.h"
#include <QDebug>
#include <QAction>
Menu_iteamOperation::Menu_iteamOperation(QWidget *parent) :
    QMenu(parent),
    ui(new Ui::Menu_iteamOperation)
{
    ui->setupUi(this);
    myAddAction();
}

Menu_iteamOperation::~Menu_iteamOperation()
{
    delete ui;
}

void Menu_iteamOperation::myAddAction(){

    // 5.6.1 必须在头文件引入QAction
    QAction *del_action =new QAction("删除", this);
    // 5.9.8
    // QAction *del_action =new QAction("删除");
    del_action->setIcon(QIcon(":/img/delete.png"));
    this->addAction(del_action);
    connect(del_action , &QAction::triggered,[=](){
          itemOperate(del);
    });
    //shao:增加重命名
    QAction *rename_action =new QAction("重命名", this);
    rename_action->setIcon(QIcon(":/img/rename.png"));
    this->addAction(rename_action);
    connect(rename_action , &QAction::triggered,[=](){
          itemOperate(rename);
    });

    QAction *property_action =new QAction("属性", this);
    property_action->setIcon(QIcon(":/img/property.png"));
    this->addAction(property_action);
    connect(property_action , &QAction::triggered,[=](){
          itemOperate(property);
    });

    QAction *edit_action =new QAction("建模", this);
    edit_action->setIcon(QIcon(":/img/editComp.png"));
    this->addAction(edit_action);
    connect(edit_action , &QAction::triggered,[=](){
          itemOperate(edit);
    });

    QAction *ppi_action =new QAction("PPI显示", this);
    ppi_action->setIcon(QIcon(":/img/windowsicon.png"));
    this->addAction(ppi_action);
    connect(ppi_action , &QAction::triggered,[=](){
          itemOperate(ppi);
    });

    QAction *ashow_action =new QAction("A显", this);
    ashow_action->setIcon(QIcon(":/img/windowsicon.png"));
    this->addAction(ashow_action);
    connect(ashow_action , &QAction::triggered,[=](){
          itemOperate(ashow);
    });
}

