#include "simupparamsetting.h"
#include "ui_simupparamsetting.h"

SimupParamSetting::SimupParamSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimupParamSetting)
{
    ui->setupUi(this);
}

SimupParamSetting::~SimupParamSetting()
{
    delete ui;
}
