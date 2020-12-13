#include "radarproperty.h"
#include "ui_radarproperty.h"

#include <QDialog>
/**
* @projectName   prototype_v0719
* @brief         雷达属性窗口类。（还没用到）
* @author        Antrn
* @date          2019-08-12
*/
RadarProperty::RadarProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RadarProperty)
{
    ui->setupUi(this);
}

RadarProperty::~RadarProperty()
{
    delete ui;
}
