#ifndef SIMUPPARAMSETTING_H
#define SIMUPPARAMSETTING_H

#include <QWidget>

namespace Ui {
class SimupParamSetting;
}

/**
 * @brief The SimupParamSetting class 仿真参数设置 （总体仿真用 还没用到）
 */
class SimupParamSetting : public QWidget
{
    Q_OBJECT

public:
    explicit SimupParamSetting(QWidget *parent = nullptr);
    ~SimupParamSetting();

private:
    Ui::SimupParamSetting *ui;
};

#endif // SIMUPPARAMSETTING_H
