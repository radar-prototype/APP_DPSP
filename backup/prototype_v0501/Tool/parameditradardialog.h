#ifndef PARAMEDITRADARDIALOG_H
#define PARAMEDITRADARDIALOG_H

#include "algorithmcomp.h"

#include <QDialog>

namespace Ui {
class ParamEditRadarDialog;
}

/**
 * @brief The ParamEditRadarDialog class是  属性值窗口（ 算法组件列表右击属性对话窗口）
 */
class ParamEditRadarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParamEditRadarDialog(QWidget *parent = nullptr);
    explicit ParamEditRadarDialog(AlgorithmComp ac, QWidget *parent = nullptr, int flag = 0/*0:新建组件，1:编辑组件*/);
    ~ParamEditRadarDialog();
    // 保存用户填写的信息
    AlgorithmComp ac;
    // 判断是新建组件还是编辑组件
    int flag = 0;
    // 保存信息
    QMap<QString, QString> mp;

private slots:
    void on_pushButton_OK_clicked();

    void on_pushButton_Cancel_clicked();

    void on_pushButton_Path_clicked();

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

signals:
    void showMessage(QString msg, int flag);
    // 给Mainwindow_radar传递信号生成icon
    void sendIconName(QString iconName);
private:
    Ui::ParamEditRadarDialog *ui;
    // 当前参数有几行
    int row=0;
    // 保存编辑文件的名字，不用。改为直接传入AlgorithmComp对象
//    QString fname;
};

#endif // PARAMEDITRADARDIALOG_H
