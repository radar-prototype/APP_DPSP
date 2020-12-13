#include "ashowdialog.h"
#include "ui_ashowdialog.h"

AshowDialog::AshowDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AshowDialog)
{
    ui->setupUi(this);
    connect(ui->radioButton, SIGNAL(clicked()),ui->widget,SLOT(startPainting()));
    connect(ui->radioButton_2, SIGNAL(clicked()),ui->widget,SLOT(stopPainting()));
}

AshowDialog::~AshowDialog()
{
    delete ui;
}
