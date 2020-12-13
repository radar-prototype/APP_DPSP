#include "ppidialog.h"
#include "ui_ppidialog.h"

PPIDialog::PPIDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PPIDialog)
{
    ui->setupUi(this);
    // 设置窗体最大化和最小化
    Qt::WindowFlags windowFlag  = Qt::Dialog;
    windowFlag                  |= Qt::WindowMinimizeButtonHint;
    windowFlag                  |= Qt::WindowMaximizeButtonHint;
    windowFlag                  |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlag);
}

PPIDialog::~PPIDialog()
{
    delete ui;
}


void PPIDialog::on_radioBtnShowOn_clicked(bool checked)
{
    if(checked)
    {
        ui->widget->start();
    }
    else
    {
        ui->radioBtnShowOn->setChecked(true);
    }
}

void PPIDialog::on_radioBtnShowOFF_clicked(bool checked)
{
    if(checked)
    {
        ui->widget->stop();
    }
    else
    {
        ui->radioBtnShowOFF->setChecked(true);
    }
}

void PPIDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
    int range;
    if(arg1 == "1.0Km")
    {
        range = 1000;
    }
    else if(arg1 == "2.0Km")
    {
        range = 2000;
    }
    else if(arg1 == "4.0Km")
    {
        range = 4000;
    }
    else
    {
        range = 6000;
    }
    ui->widget->setRange(range);
}

void PPIDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->widget->setImageSize(value);
}

void PPIDialog::on_radioButtonNormal_clicked(bool checked)
{
    if(checked)
    {
        ui->widget->setShowMode(Normal);
    }
    else
    {
        ui->radioButtonNormal->setChecked(true);
    }
}

void PPIDialog::on_radioButtonOffCenter_clicked(bool checked)
{
    if(checked)
    {
        ui->widget->setShowMode(OffCenter);
    }
    else
    {
        ui->radioButtonOffCenter->setChecked(true);
    }
}

void PPIDialog::on_radioButtonBShow_clicked(bool checked)
{
    if(checked)
    {
        ui->widget->setShowMode(BShow);
    }
    else
    {
        ui->radioButtonBShow->setChecked(true);
    }
}
