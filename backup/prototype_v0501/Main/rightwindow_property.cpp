#include "rightwindow_property.h"
#include "ui_rightwindow_property.h"

RightWindow_Property::RightWindow_Property(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RightWindow_Property)
{
    ui->setupUi(this);
}

RightWindow_Property::~RightWindow_Property()
{
    delete ui;
}
