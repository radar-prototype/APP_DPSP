#include "myglwidgwt.h"
#include "ui_myglwidgwt.h"

myGLWidgwt::myGLWidgwt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myGLWidgwt)
{
    ui->setupUi(this);
}

myGLWidgwt::~myGLWidgwt()
{
    delete ui;
}
