#ifndef ASHOWDIALOG_H
#define ASHOWDIALOG_H

#include <QDialog>

namespace Ui {
class AshowDialog;
}
// A显 弹窗 （总体仿真 雷达组件右击  点击A显 弹出窗口）
class AshowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AshowDialog(QWidget *parent = nullptr);
    ~AshowDialog();

private:
    Ui::AshowDialog *ui;
};

#endif // ASHOWDIALOG_H
