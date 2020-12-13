#ifndef RUNCOMPCONF_H
#define RUNCOMPCONF_H

#include "qcustomplot.h"
#include <QDialog>

namespace Ui
{
class RunCompConf;
}
/**
 * @brief The RunCompConf class 算法组件配置（运行窗口  调用了第三方画图工具）
 */
class RunCompConf : public QDialog
{
    Q_OBJECT

public:
    explicit RunCompConf(QString fp, QWidget *parent = nullptr);
    ~RunCompConf();
    //设置qcustomplot画图属性，实时
    void setupRealtimeDataDemo(QCustomPlot *customPlot);
    void readFile(QString filePath);

signals:
    void rateSignal(float rate);
    void overRun();
    void startRun();

public slots:
    void On_start();
    void On_pause();
    void On_over();
    void On_rateSignal(float rate);
private slots:
    //添加实时数据槽
    void realtimeDataSlot();

private:
    Ui::RunCompConf *ui;
    //定时器，周期调用realtimeDataSlot()槽，实现动态数据添加到曲线
    QTimer dataTimer;
    QVector<double> realArray;
    int i = 0;
    QString dataPath;
};

#endif // RUNCOMPCONF_H
