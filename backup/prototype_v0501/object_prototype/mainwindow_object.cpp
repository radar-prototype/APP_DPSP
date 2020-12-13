#include "mainwindow_object.h"
#include "ui_mainwindow_object.h"

MainWindow_object::MainWindow_object(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_object)
{
    ui->setupUi(this);
}

MainWindow_object::~MainWindow_object()
{
    delete ui;
}
