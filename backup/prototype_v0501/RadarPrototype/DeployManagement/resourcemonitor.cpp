#include "resourcemonitor.h"
#include "ui_resourcemonitor.h"
#include <QProgressBar>
#include <QLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QListWidget>
#include <QRegExp>
#include "requestMsg.h"
#include <QFile>
#include <QtXml/QtXml>
#include <QtXml/QDomDocument>
#include <QMessageBox>

ResourceMonitor::ResourceMonitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResourceMonitor)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    //resize(800,600);
    // fan
    // QChart 改为 UsageRateChart
    //chartView = new QChartView;
    usageRateChart = new UsageRateChart;
    usageRateChart->setStyleSheet("background-color:white;");
    QVBoxLayout *chartLayout = new QVBoxLayout;
    chartLayout->addWidget(usageRateChart);
    ui->groupBox->setLayout(chartLayout);
    //设置定时器，每个1秒刷新状态
    timer = new QTimer;
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateData()));
    //获取节点信息
    if(!initState())
    {
        qDebug() << "初始化状态失败，请检查服务器配置";
    }
    else
    {
        this->show();
        //绘制折线图表
        //drawCharts();
        usageRateChart->updateChart(hardwares[visible].points);
        //绘制进度条
        QVBoxLayout *scrollLayout = new QVBoxLayout(ui->scrollArea->widget());
        for(int i = 0; i < hardwares.size(); ++i)
        {
            //设置组合框与进度条
            hardwares[i].groupBox = new QGroupBox;
            hardwares[i].groupBox->setTitle(hardwares[i].type);
            hardwares[i].groupBox->setFixedHeight(80);
            hardwares[i].groupBox->setCheckable(true);

            hardwares[i].progressBar = new QProgressBar;
            hardwares[i].progressBar->setValue(hardwares[i].ratio);

            QVBoxLayout *groupLayout = new QVBoxLayout;
            groupLayout->addWidget(hardwares[i].progressBar);
            hardwares[i].groupBox->setLayout(groupLayout);
            scrollLayout->addWidget(hardwares[i].groupBox, 0, Qt::AlignTop);

            if(i == 0)
            {
                hardwares[i].groupBox->setChecked(true);
                checkedGroupBox = hardwares[i].groupBox;
                ui->groupBox->setTitle(hardwares[i].type);
            }
            else
            {
                hardwares[i].groupBox->setChecked(false);
            }

            connect(hardwares[i].groupBox, &QGroupBox::clicked, [ = ](bool checked)
            {
                if(checked)
                {
                    checkedGroupBox->setChecked(false);
                    hardwares[i].groupBox->setChecked(true);
                    checkedGroupBox = hardwares[i].groupBox;
                    //替换折线
                    //qobject_cast<QSplineSeries *>(monitorChart->series().first())->replace(hardwares[i].points);
                    //series->replace(hardwares[i].points);
                    ui->groupBox->setTitle(hardwares[i].type);
                    visible = i;
                    //this->updateSeries();
                    usageRateChart->updateChart(hardwares[i].points);
                }
                else
                {
                    hardwares[i].groupBox->setChecked(true);
                }
            });
            //hardwares[i].groupBox->setToolTip("0%");
        }

        scrollLayout->addStretch();

        //drawCharts();
        //反走样
        //chartView->setRenderHint(QPainter::Antialiasing);
        //默认显示第一个图表
        //chartView->setChart(monitorChart);
        timer->start();
    }

}

ResourceMonitor::~ResourceMonitor()
{
    for(int i = 0; i < nodes.size(); ++i)
    {
        delete buffer[i];
    }
    delete ui;
}

/*
void ResourceMonitor::drawCharts()
{

    monitorChart = new QChart();
    monitorChart->setMargins(QMargins(5, 5, 5, 5));
    series = new QSplineSeries(monitorChart);
    area = new QAreaSeries(monitorChart);
    area->setName("%利用率");
    *series << QPoint(0, 0);
    area->setUpperSeries(series);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::blue);
    //pen.setColor(QColor().fromRgb(193,210,240));
    series->setPen(pen);

    series->setUseOpenGL(true);
    monitorChart->addSeries(area);

    //设置坐标轴
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 60);
    axisX->setLabelFormat("%g");
    axisX->setMinorTickCount(20);
    axisX->setTickCount(2);
    //axisX->setReverse();
    QValueAxis *axisY = new QValueAxis;
    axisY->setMax(100);
    //axisY->setRange(0,100);
    axisY->setLabelFormat("%g");
    axisY->setMinorTickCount(10);
    axisY->setTickCount(2);

    monitorChart->setAxisX(axisX, area);
    monitorChart->setAxisY(axisY, area);
}
*/

//更新数据
void ResourceMonitor::updateData()
{
    //char *buffer[10];
    //buffer[0] = static_cast<char *>(malloc(sizeof(char)*100));

    // NOTE 需要链接库
    //getStatus(buffer);
    QStringList status = QString(buffer[nodeIndex]).split('\n');
    status.removeLast();
    if(!ClusterUtils::nodeIsAvailable(nodeIndex))
        if(QMessageBox::warning(this, "WARNING!", "无法连接到服务器！") == QMessageBox::Ok)
            this->close();
    //QString status(buffer[0]);
    //delete buffer[0];
    //解析通信中间件信息
    QRegExp regExp("[0-9]+\.[0-9]{2}");
    //regExp.indexIn(status);
    for(int i = 0; i < hardwares.size(); ++i)
    {
        //hardwares[i].ratio = qAbs(100 + qrand() % 3 - 1);
        //获取利用率
        regExp.indexIn(status[i]);
        hardwares[i].ratio = static_cast<int>(regExp.cap().toDouble() + 0.5);
        hardwares[i].progressBar->setValue(hardwares[i].ratio);
        //对点的坐标左移调整
        //QVector<QPointF> vector = hardwares[i].series->pointsVector();
        if(hardwares[i].points.size() == 61)
            hardwares[i].points.removeFirst();

        for(QPointF &p : hardwares[i].points)
            ++(p.rx());

        hardwares[i].points.append(QPointF(0, hardwares[i].ratio));
    }
    //更新折线
    //updateSeries();
    usageRateChart->updateChart(hardwares[visible].points);
}

/*
//更新对应硬件的Series
void ResourceMonitor::updateSeries()
{
    //替换上界折线
    series->replace(hardwares[visible].points);

    //更新折线区域
    QValueAxis *axisX = qobject_cast<QValueAxis *>(monitorChart->axisX());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(monitorChart->axisY());
    //axisX->setReverse();
    //delete area;
    monitorChart->removeAllSeries();
    QAreaSeries *area = new QAreaSeries(monitorChart);
    area->setName("%利用率");
    area->setUpperSeries(series);
    area->setOpacity(0.5);

    //设置边界颜色和填充颜色
    area->setColor(QColor().fromRgb(241, 246, 250));
    area->setBorderColor(QColor().fromRgb(39, 137, 193));
    monitorChart->addSeries(area);
    monitorChart->setAxisX(axisX, area);
    monitorChart->setAxisY(axisY, area);
}
*/

//初始化状态
bool ResourceMonitor::initState()
{
    //读取rpc.xml文件
    QDomDocument doc;
    QFile rpcXml("./rpc.xml");
    if (!rpcXml.open(QIODevice::ReadOnly))
        return false;
    if(!doc.setContent(&rpcXml))
        rpcXml.close();

    QDomElement root = doc.documentElement();
    //读取第一个node节点
    QDomNode node = doc.documentElement().firstChild();
    while(!node.isNull())
    {
        if(node.firstChild().toElement().tagName() == "server-IP")
            nodes.append(Node(node.firstChild().toElement().text(), true));
        node = node.nextSibling();
    }
    rpcXml.close();

    //从服务端读取信息
    for(int i = 0; i < nodes.size(); ++i)
    {
        buffer[i] = static_cast<char *>(malloc(sizeof(char) * 100));
    }

    // NOTE 需要配置链接库
    //getStatus(buffer);
    //QString status = buffer[0];
    QStringList status = QString(buffer[0]).split("\n");
    status.removeLast();
    //检查是否能获得节点信息
    if(status.empty())
    {
        QMessageBox::warning(nullptr, "WARNING!", "无法连接到服务器！");
        // Antrn: 此行导致窗口闪烁
//        this->close();
        return false;
    }
    // Antrn: 不加判断导致数组溢出
    else
    {
        //解析硬件类型
        for(QString s : status)
        {
            QString type = s.section(' ', 0, 0);
            hardwares.append(Hardware(type));
            hardwares.back().points.append(QPoint(0, 0));
        }
        //设置节点选择下拉框
        nodeComboBox = new QComboBox(this);
        ui->horizontalLayout_2->addWidget(nodeComboBox);
        for(int i = 0; i < nodes.size(); ++i)
        {
            nodeComboBox->addItem(QString("节点%1").arg(i));
            //ui->node_comboBox->addItem(QString("节点 %1").arg(i));
        }
        connect(nodeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeNode(int)));
        ui->ip_label->setText(nodes[nodeIndex].nodeIP);
        return true;
    }
    connect(nodeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeNode(int)));
    ui->ip_label->setText(nodes[nodeIndex].nodeIP);
}

void ResourceMonitor::changeNode(int index)
{
    if(!ClusterUtils::nodeIsAvailable(index))
    {
        QMessageBox::warning(this, "WARNING!", "无法连接到服务器！");
        nodeComboBox->setCurrentIndex(nodeIndex);
        return;
    }
    nodeIndex = index;
    for(int i = 0; i < nodes.size(); ++i)
    {
        hardwares[i].points.clear();
        hardwares[i].points.append(QPoint(0, 0));
    }

    hardwares[visible].groupBox->setChecked(false);
    hardwares[0].groupBox->setChecked(true);
    visible = 0;
    ui->ip_label->setText(nodes[nodeIndex].nodeIP);
    //updateSeries();
    usageRateChart->updateChart(hardwares[visible].points);
}
