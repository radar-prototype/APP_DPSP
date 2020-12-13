#ifndef COMP_THREAD_H
#define COMP_THREAD_H

#include <QDialog>

namespace Ui
{
class comp_thread;
}

class comp_thread : public QDialog
{
    Q_OBJECT

public:
    explicit comp_thread(QWidget *parent = nullptr);
    ~comp_thread();
    //显示组件名称
    void setname(QString name);
    //显示组件ID
    void setID(QString ID);

signals:
    //向mainwindow_radar传递线程数
    void threadnum_inserted(int num);

private slots:
    //确定键槽函数
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::comp_thread *ui;
};

#endif // COMP_THREAD_H
