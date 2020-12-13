#include "main_config_dialog.h"
#include "ui_main_config_dialog.h"
#include <QObject>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
// This is an auto-generated comment for this class.
/********************************************************************************************
 * @author Antrn
 * @motto Done is better than Perfect, Talk is cheap. Show me the code.
 * @date 2020-10-15 T 16:40:27
 * @description
 * @copyright (c)
 ********************************************************************************************/
Main_config_dialog::Main_config_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Main_config_dialog)
{
    ui->setupUi(this);
    // Antrn: FIXME 按钮后续打开，也支持用户选择路径
    ui->pushButton->setVisible(false);
    // Antrn: 日志文件地址标签label
    ui->label_4->setVisible(false);
}

Main_config_dialog::~Main_config_dialog()
{
    delete ui;
}

//shao:设置默认值（注意服从规则）()
void Main_config_dialog::set_default_value(QMap<QString, QString> parameter_list)
{
    QMap<QString, QString>::Iterator it;
    QFormLayout *pLayout = new QFormLayout();
    for ( it = parameter_list.begin(); it != parameter_list.end(); ++it )
    {
        // Antrn: FIXME 这个配置路径后面要打开
//        if(QString::compare(it.key(), "path") == 0)
//        {
//            ui->pushButton->setText(it.value());
//        }
        if(QString::compare(it.key(), "max_thread") == 0)
        {
            ui->max_thread->setValue(it.value().toInt());
        }
        else if(QString::compare(it.key(), "msg_len") == 0)
        {
            ui->msg_len->setValue(it.value().toInt());
        }
        else if(QString::compare(it.key(), "msg_size") == 0)
        {
            ui->msg_size->setValue(it.value().toInt());
        }
        else if(QString::compare(it.key(), "file_path") == 0)
        {
            ui->file_path->setText(it.value());
        }
        else
        {
            //shao:剩下的是组件的最大线程数
            QLabel *lable = new QLabel(this);
            lable->setText(it.key());
            QSpinBox* spinBox = new QSpinBox(this);
            spinBox->setValue(it.value().toInt());
            pLayout->addRow(lable, spinBox);
        }
        ui->groupBox->setLayout(pLayout);
    }
}

//shao:得到参数
QMap<QString, QString> Main_config_dialog::get_parameter_value()
{
    QMap<QString, QString>  parameter_list;
    parameter_list.insert("max_thread", QString::number(ui->max_thread->value()));
    parameter_list.insert("msg_len", QString::number(ui->msg_len->value()));
    parameter_list.insert("msg_size", QString::number(ui->msg_size->value()));
//    parameter_list.insert("path", ui->pushButton->text());
    parameter_list.insert("file_path", ui->file_path->text());

    QList<QSpinBox*>list_v = ui->groupBox->findChildren<QSpinBox*>();
    QList<QLabel*>list_k = ui->groupBox->findChildren<QLabel*>();
    for(int i = 0 ; i < list_k.length(); i++)
    {
        parameter_list.insert(list_k.at(i)->text(), QString::number(list_v.at(i)->value()));
    }
    return parameter_list;
}

void Main_config_dialog::on_pushButton_clicked()
{
    QString dirpath = QDir::currentPath();
    QString path = QFileDialog::getExistingDirectory(this, "选择文件夹", QString(dirpath), QFileDialog::ShowDirsOnly);
    ui->pushButton->setText(path);
}
void Main_config_dialog::accept()
{
//    qDebug()<<"ok"<<"11111111111111111111";
//    this->close();
    // Antrn: TODO 在生成的target目录中生成files文件夹
    emit this->setup_complete();
}
void Main_config_dialog::rejected()
{
    this->close();
}

