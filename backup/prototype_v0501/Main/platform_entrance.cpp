#include "platform_entrance.h"
#include "ui_platform_entrance.h"

Platform_entrance::Platform_entrance(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Platform_entrance)
{
    ui->setupUi(this);
}

Platform_entrance::~Platform_entrance()
{
    delete ui;
}
// 点击 雷达样机 按钮
void Platform_entrance::on_pushButton_radar_clicked(bool checked)
{
    if(checked){
        //跳转 编辑页面 （有个问题怎么跳回来）

    }

}
