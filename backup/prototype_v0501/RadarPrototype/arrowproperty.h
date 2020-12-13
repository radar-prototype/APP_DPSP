#ifndef ARROWPROPERTY_H
#define ARROWPROPERTY_H
/**
* @brief         箭头属性窗口类
*/
#include <QDialog>
#include <QVariant>
namespace Ui {
class arrowproperty;
}

class arrowproperty : public QDialog
{
    Q_OBJECT

public:
    explicit arrowproperty(QWidget *parent = nullptr);
    //设置属性窗口中组件和端口的值
    void setArrow(QVariant);
    ~arrowproperty();

private slots:
    void on_pushButton_clicked();

private:
    Ui::arrowproperty *ui;
};

#endif // ARROWPROPERTY_H
