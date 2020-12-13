#include "ecmproperty.h"
#include "ui_ecmproperty.h"

EcmProperty::EcmProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EcmProperty)
{
    ui->setupUi(this);
}

EcmProperty::~EcmProperty()
{
    delete ui;
}
