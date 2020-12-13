#include "comp_cpu.h"
#include "ui_comp_cpu.h"
#include <QDebug>

comp_cpu::comp_cpu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::comp_cpu)
{
    ui->setupUi(this);
}

comp_cpu::~comp_cpu()
{
    delete ui;
}

void comp_cpu::on_pushButton_clicked()
{
    this->hide();
}

//显示组件名称
void comp_cpu::setname(QString name)
{
    ui->lineEdit_compname->setText(name);
}

//显示组件ID
void comp_cpu::setID(QString ID)
{
    ui->lineEdit_compID->setText(ID);
}

//先选择发送到节点的IP地址才能选择使用的cpu
void comp_cpu::on_comboBox_currentTextChanged(const QString &arg1)
{
    //qDebug() <<"测试：当前测试框已有值被选中" << arg1;
    ui->comboBox_cpuchoose->setEnabled(true);
    ip = arg1;
}
//先选择使用的CPU再选择使用的核心
void comp_cpu::on_comboBox_cpuchoose_currentTextChanged(const QString &arg1)
{
    ui->comboBox_corechoose->setEnabled(true);
}

void comp_cpu::on_pushButton_2_clicked()
{
    //传送IP地址
    emit IPaddress_choosed(ip);
    this->hide();
}
