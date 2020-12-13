#ifndef ECHOESWAVE_H
#define ECHOESWAVE_H

#include "qcustomplot.h"

#include <QMainWindow>

namespace Ui {
class EchoesWave;
}
/**
 * @brief The EchoesWave class 生成波形窗口 （组件右击波形显示窗口）
 */
class EchoesWave : public QMainWindow
{
    Q_OBJECT

public:
    explicit EchoesWave(QWidget *parent = nullptr);
    ~EchoesWave();
    //设置qcustomplot画图属性，实时
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

private slots:
    //添加实时数据槽
    void realtimeDataSlot();

private:
    Ui::EchoesWave *ui;
    //定时器，周期调用realtimeDataSlot()槽，实现动态数据添加到曲线
    QTimer dataTimer;
};

#endif // ECHOESWAVE_H
