#ifndef COMP_CPU_H
#define COMP_CPU_H

#include <QDialog>

namespace Ui
{
class comp_cpu;
}

class comp_cpu : public QDialog
{
    Q_OBJECT

public:
    explicit comp_cpu(QWidget *parent = nullptr);
    ~comp_cpu();
    //显示组件名称
    void setname(QString name);
    //显示组件ID
    void setID(QString ID);

signals:
    void IPaddress_choosed(QString ip);

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_comboBox_cpuchoose_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::comp_cpu *ui;
    //用户选择的IP地址
    QString ip;
};

#endif // COMP_CPU_H
