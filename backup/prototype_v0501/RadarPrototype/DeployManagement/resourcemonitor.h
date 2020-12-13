#ifndef RESOURCEMONITOR_H
#define RESOURCEMONITOR_H

#include <QDialog>
#include <QTimer>
#include <QGroupBox>
#include <QComboBox>
#include <QProgressBar>
#include "clusterutils.h"
#include "usageratechart.h"

namespace Ui
{
class ResourceMonitor;
}

//节点中的硬件信息
struct Hardware
{
    QString type;
    int ratio;

    QGroupBox *groupBox;
    QProgressBar *progressBar;
    //QChart *chart;
    QVector<QPointF> points;

    Hardware(QString t, int r = 0) : type(t), ratio(r) {}
};

/*
struct Node
{
    QString nodeIP;
    bool available;
    //QList<Hardware> hardwares;
    Node(QString s, bool b) : nodeIP(s), available(b) {}
};
*/

class ResourceMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit ResourceMonitor(QWidget *parent = nullptr);
    ~ResourceMonitor();
    bool initState();
    //void updateSeries();

public slots:
    //void drawCharts();
    void updateData(); //从通信中间件获取数据并更新状态

private slots:
    void changeNode(int index);

private:
    Ui::ResourceMonitor *ui;

    QTimer *timer;
    int visible = 0;//当前显示的硬件序号
    int nodeIndex = 0;//选中的节点

    //QChartView *chartView;
    //QChart *monitorChart;
    //QSplineSeries *series;
    //QAreaSeries *area;

    QList<Hardware> hardwares;//选中节点硬件信息
    QList<Node> nodes; //节点

    QGroupBox *checkedGroupBox;
    QComboBox *nodeComboBox;//节点选择下拉框
    char *buffer[10];

    UsageRateChart *usageRateChart;
};

#endif // RESOURCEMONITOR_H
