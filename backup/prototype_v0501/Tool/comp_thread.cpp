#include "comp_thread.h"
#include "ui_comp_thread.h"
#include "diagramitem.h"

comp_thread::comp_thread(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::comp_thread)
{
    ui->setupUi(this);
}

comp_thread::~comp_thread()
{
    delete ui;
}

//显示组件ID
void comp_thread::setID(QString ID)
{
    ui->lineEdit_compID->setText(ID);
}
//显示组件名称
void comp_thread::setname(QString name)
{
    ui->lineEdit_compname->setText(name);
}
//确定键槽函数，设置当前组件所使用的的线程数量
void comp_thread::on_pushButton_2_clicked()
{
    //输入线程数量
    int num = ui->lineEdit_threadnum->text().toInt();
    emit threadnum_inserted(num);

    this->hide();
}
//返回键槽函数
void comp_thread::on_pushButton_clicked()
{
    this->hide();
}
