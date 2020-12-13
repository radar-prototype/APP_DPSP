#ifndef MAIN_CONFIG_dialog_H
#define MAIN_CONFIG_dialog_H

#include <QObject>
#include <QDialog>
//shao:代码生成总体配置窗口
namespace Ui
{
class Main_config_dialog;
}

class Main_config_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Main_config_dialog(QWidget *parent = nullptr);
    ~Main_config_dialog();
    //shao:设置默认值（注意服从规则）
    void set_default_value(QMap<QString, QString> parameter_list);
    //shao:得到参数
    QMap<QString, QString>  get_parameter_value();
private:
    Ui::Main_config_dialog *ui;
signals:
    //完成的时候发送
    void setup_complete();
private slots:
    void on_pushButton_clicked();
    void accept();
    void rejected();
};

#endif // MAIN_CONFIG_dialog_H
