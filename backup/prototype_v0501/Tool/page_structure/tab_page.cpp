#include "tab_page.h"
#include "ui_tab_page.h"
#include <QDebug>

Tab_page::Tab_page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab_page)
{
    ui->setupUi(this);
    this->ui->tabWidget->setMovable(true);
    this->ui->tabWidget->setTabsClosable(true);
    connect(this->ui->tabWidget,&QTabWidget::tabCloseRequested,this,[this](int index){
        bool if_successful;
        //shao:没成功，下面的方式出问题以后再试试吧
        //        QMetaObject::invokeMethod(this, "tabCloseRequested", Qt::BlockingQueuedConnection,
        //                                  Q_RETURN_ARG(bool, if_successful),
        //                                  Q_ARG(QWidget*, this->ui->tabWidget->widget(index)));
        emit this->tabCloseRequested(this->ui->tabWidget->widget(index), &if_successful);
        if(if_successful){
            this->ui->tabWidget->removeTab(index);
        }else{
            qDebug()<<"没保存，所以没关闭页面";
        }

    });
}

Tab_page::~Tab_page()
{
    delete ui;
}
bool Tab_page::add_page(QWidget *page,QString name){
    int index =this->ui->tabWidget->addTab(page,name);
    this->ui->tabWidget->setCurrentIndex(index);
    return true;
}
bool Tab_page::remove_page(int index){
    if(index==-1){
        this->ui->tabWidget->removeTab(1);
        this->ui->tabWidget->removeTab(0);
    }else{
        //    page----》index
        //    int index=0;
          this->ui->tabWidget->removeTab(index);
    }
    return true;
}
bool Tab_page::swith_page(QWidget *page){
    this->ui->tabWidget->setCurrentWidget(page);
    return true;
}
//-1:不存在，其他数字：存在，返回当前的index
int Tab_page::index_of(QWidget *page){
    return this->ui->tabWidget->indexOf(page);
}
bool Tab_page::rename_page(QWidget *page,QString new_name){
    int index=this->index_of(page);
    if(index==-1) return false;
    this->ui->tabWidget->setTabText(index,new_name);
    return true;
}
