#include "mainwindow.h"
#ifndef HELLODIALOG_H
#define HELLODIALOG_H

/**
* @projectName   prototype_v0719
* @brief         开始欢迎页面，也即是进入主程序之前的拦截界面。可以选择打开或者新建工程。
* @author        Antrn
* @date          2019-08-12
*/
#include <QDialog>

namespace Ui
{
class HelloDialog;
}

class HelloDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelloDialog(QWidget *parent = nullptr);
    bool createProListFile();

private slots:

    void on_pushButton_open_clicked();

    void on_pushButton_new_clicked();

private:
    Ui::HelloDialog *ui;
//    MainWindow *main;//shao:修改成同一窗口的样式之后就不用了
signals:
    void set_window(int a, int b, QWidget* Widget);
};
#endif // HELLODIALOG_H
