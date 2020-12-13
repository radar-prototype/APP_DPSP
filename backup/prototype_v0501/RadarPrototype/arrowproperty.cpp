#include "arrowproperty.h"
#include "ui_arrowproperty.h"
#include "arrow.h"
#include <QDebug>
/**
* @brief         箭头属性窗口类
*/
arrowproperty::arrowproperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::arrowproperty)
{
    ui->setupUi(this);

}

arrowproperty::~arrowproperty()
{
    delete ui;
}

void arrowproperty::setArrow(QVariant variant)
{
    Arrow_Data arrow_data=variant.value<Arrow_Data>();
    //获取组件和端口的值
    ui->lineEdit_begincomp->setText(arrow_data.begin_comp);
    ui->lineEdit_beginport->setText(arrow_data.begin_port);
    ui->lineEdit_endcomp->setText(arrow_data.end_comp);
    ui->lineEdit_endport->setText(arrow_data.end_port);
    //获取箭头ID
    ui->lineEdit_ID->setText(arrow_data.ID);
}

void arrowproperty::on_pushButton_clicked()
{
    this->close();
}
