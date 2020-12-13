#include "realtimesignal.h"
#include "ui_realtimesignal.h"
#include <QDebug>
#include <qmath.h>
#include <QColor>
#include <QProcess>


RealTimeSignal::RealTimeSignal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RealTimeSignal)
{

    //外部启动接收数据命令
//    QProcess receive_process(this);

//    receive_process.setProgram("cmd");
//    QStringList order;
//    order << "sh DCPS.sh";
//    receive_process.setArguments(order);
//    receive_process.start();
//    receive_process.waitForStarted();
//    receive_process.waitForFinished();

    //初始化参数
    ui->setupUi(this);
    ui->lineEdit->setText(QString::number(point_num));
    ui->lineEdit_offset->setText(QString::number(offset*8));
    ui->lineEdit_speed->setText(QString::number(time_space));
    ui->radioButton_realpart->isChecked();
    if(!is_realnumber)
    {
        ui->radioButton_alternate->setEnabled(true);
        ui->radioButton_sidebyside->setEnabled(true);
    }
    //初始化绘图界面
    QCustomPlot *customplot = ui->widget;
//    customplot->setBackground(QColor(0,0,0,0.5));
    customplot->axisRect()->setBackground(QBrush(QColor(0,0,0)));
    customplot->xAxis->setTickLabelColor(QColor(100,100,100));
    customplot->yAxis->setTickLabelColor(QColor(100,100,100));
    customplot->xAxis->grid()->setPen(QColor(50,50,50));
    customplot->yAxis->grid()->setPen(QColor(50,50,50));
    customplot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    customplot->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    customplot->xAxis->setBasePen(QPen(QColor(200,200,200)));
    customplot->yAxis->setBasePen(QPen(QColor(200,200,200)));
    customplot->xAxis->setTickPen(QColor(200,200,200));
    customplot->yAxis->setTickPen(QColor(200,200,200));
    customplot->xAxis->setSubTickPen(QColor(200,200,200));
    customplot->yAxis->setSubTickPen(QColor(200,200,200));
    connect(customplot,&QCustomPlot::sendLeft,this,&RealTimeSignal::receiveLeft);
    connect(customplot,&QCustomPlot::sendRight,this,&RealTimeSignal::receiveRight);

    QString message1 = "中间件发送"+QString::number(time_space)+"组数据后将一组数据进行展示";
    ui->lineEdit_speed->setToolTip(message1);

    /* weishi:将大数组的内容存到小数组的过程使用另一个线程进行计算
     * 开启处理线程
     * */
    myworker = new worker();
    myworker->moveToThread(&myWorkThread);

    connect(this, &RealTimeSignal::startRunning,myworker,&worker::do_work);
    connect(&myWorkThread,&QThread::finished,myworker,&QObject::deleteLater);
    connect(myworker, &worker::work_finished, this, &RealTimeSignal::receive_result);

    qRegisterMetaType<QList<double>>("QList<double>");//对信号startRunning传递数组Y进行注册，需要在信号和槽的定义位置进行注册
    qRegisterMetaType<QVector<double>>("QVector<double>");


    myWorkThread.start();
    /*
     * 开启接收线程,负责接收中间件发送的数据
     */
    rt = new ReceiveThread;
    connect(rt, &ReceiveThread::sendAddressAndLen, this, &RealTimeSignal::handleData);
}

RealTimeSignal::~RealTimeSignal()
{

    myWorkThread.quit();
    myWorkThread.wait();
    delete ui;
}


//点击运行按钮,对显示页面进行初始化
void RealTimeSignal::on_pushButton_run_clicked()
{
    flag_canDrawGraph = true;
    time_space = ui->lineEdit_speed->text().toInt();//初始化刷新速度
    point_num = ui->lineEdit->text().toInt();//初始化展示点数
    offset = ui->lineEdit_offset->text().toInt()/8;//初始化偏移量,将位转化为字节
    Right = point_num;//放大后的右边边界值
    rt->setReceivedata_interval(time_space);
    setupRealtimeDataDemo();//初始化界面

    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_pause->setEnabled(true);
}
//暂停槽函数
void RealTimeSignal::on_pushButton_pause_clicked()
{
    flag_canDrawGraph = false;
//    timer.stop();
}
//停止键槽函数
void RealTimeSignal::on_pushButton_stop_clicked()
{
    flag_canDrawGraph = false;
//    timer.stop();
    myWorkThread.quit();
    myWorkThread.wait();
    // weishi:当停止键按下后，用户只能选择退出界面，不能进行其他操作
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_pause->setEnabled(false);
}
//weishi:初始化界面
void RealTimeSignal::setupRealtimeDataDemo()
{
    QCustomPlot *customplot = ui->widget;
    customplot->addGraph();//添加曲线
    customplot->setInteraction(QCP::iRangeZoom);
    //customplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    rt->start();

    //鼠标移动触发槽函数显示坐标点的坐标
    connect(customplot, &QCustomPlot::mouseMove,this, &RealTimeSignal::myMoveEvent);

    customplot->xAxis->setRange(1,point_num);
    customplot->yAxis->setRange(0,10);
    customplot->graph()->setPen(QPen(QColor(200,200,200)));
}

//接收处理过后的纵坐标数据,结合横坐标进行绘图
void RealTimeSignal::receive_result(QVector<double> yValue)
{
    // 横坐标
    QVector<double> xValue;
    //
    int num = 0;
    while(num < this->point_num)
    {
        xValue.append(num);
        if(num%2000==0)
            qDebug() << "数量-横坐标-y坐标:" << num <<xValue[num] <<yValue[num];
        num++;
    }
    QCustomPlot *customplot = ui->widget;
    customplot->graph(0)->setData(xValue, yValue);
    customplot->graph(0)->rescaleAxes();
    customplot->replot();
}
// 接收中间件发送的原始数据,交给数据处理线程进行处理
void RealTimeSignal::handleData(char * add, int len)
{
//    qDebug() <<"======buffer地址====="<<(void*)&add;
    //确定用户需要将原始数据处理成什么样的类型
    QString drawdata_type;
    if(is_realnumber)
    {
        drawdata_type = "real";//实数
    }
    else
    {
        if(is_alternate)
        {
            if(is_drawmodel)
            {
                drawdata_type = "fualternate_model";//复数交替排列求模
            }
            else if(is_drawreal)
            {
                drawdata_type = "fualternate_real";//复数交替排列实部
            }
            else if(is_drawimagineary)
            {
                drawdata_type = "fualternate_imagineary";//复数交替排列虚部
            }
        }
        else if(is_sidebyside)
        {
            if(is_drawmodel)
            {
                drawdata_type = "fusidebyside_model";//复数并列排列求模
            }
            else if(is_drawreal)
            {
                drawdata_type = "fusidebyside_real";//复数并列排列实部
            }
            else if(is_drawimagineary)
            {
                drawdata_type = "fusidebyside_imagineary";//复数并列排列虚部
            }
        }
    }
    if(drawdata_type == "NULL")
    {
        qDebug() <<"绘制类型没有记录，请重新检查 ";
    }
    //获取去除掉数据头部的有效数据地址
    char *correct_buffer = add + offset;
    qDebug() <<"承接到的备份数buffer地址"<<(void*)&add<<"增加偏移量后地址"<<(void*)&correct_buffer;
    //本次接收数据的数据类型
    QString data_type = ui->comboBox->currentText();
    //drawdata_type表示绘制哪一部分的图像,data_type表示数据的类型是short或是其他

    if(flag_canDrawGraph)
        emit startRunning(correct_buffer, len - offset, drawdata_type, data_type, this->point_num, save_data);//drawdata_type表示绘制哪一部分的图像
    //this->receiveData(add, len);
    free(add);
    add = NULL;

}

void RealTimeSignal::myMoveEvent(QMouseEvent *event)
{
    QCustomPlot *custom = ui->widget;
    //获取横纵坐标
    int x_pos = event->pos().x();
    int y_pos = event->pos().y();


    //鼠标坐标转化为CustomPlot内部坐标
    float x_val = custom->xAxis->pixelToCoord(x_pos);
    qreal x =x_val;// 四舍五入操作
    float y_val = custom->yAxis->pixelToCoord(y_pos);

    QString str,strToolTip;
    str = QString::number(qRound(x), 10, 3);
    strToolTip += "x: ";
    strToolTip += str;
    strToolTip += "\n";

    for (int i = 0;i < custom->xAxis->graphs().count();i++) {
           //获得x轴坐标位置对应的曲线上y的值
           double y = custom->graph(i)->data()->at(qRound(x))->value;
           str = QString::number(y);
       }
    strToolTip += "y" + QString::number(0) + "：";
    strToolTip += str;
    strToolTip += "\n";
    QToolTip::showText(cursor().pos(), strToolTip, custom);
}

void RealTimeSignal::on_lineEdit_speed_returnPressed()
{
    this->time_space = ui->lineEdit_speed->text().toInt();
}


//设置当前页面显示的点的数量
void RealTimeSignal::on_lineEdit_returnPressed()
{
    point_num = ui->lineEdit->text().toInt();
}

void RealTimeSignal::on_comboBox_activated(const QString &arg1)
{
    Q_UNUSED(arg1)
    data_type = ui->comboBox->currentText();
    qDebug() <<data_type;
}

void RealTimeSignal::on_lineEdit_offset_returnPressed()
{
    offset = ui->lineEdit_offset->text().toInt();
}

void RealTimeSignal::on_radioButton_imaginarypart_clicked()
{
    is_realnumber = false;
    ui->radioButton_alternate->setEnabled(true);
    ui->radioButton_sidebyside->setEnabled(true);
}

void RealTimeSignal::on_radioButton_alternate_clicked()
{
    is_alternate = true;//复数的存储形式为交替排列
    is_sidebyside = false;
    //ui->radioButton_sidebyside->setEnabled(false);
    ui->radioButton_drawreal->setEnabled(true);
    ui->radioButton_drawimagine->setEnabled(true);
    ui->radioButton_drawmodel->setEnabled(true);
}

void RealTimeSignal::on_radioButton_drawreal_clicked()
{
    is_drawreal = true;//描绘实部
    is_drawimagineary = false;
    is_drawmodel = false;
}

void RealTimeSignal::on_radioButton_drawimagine_clicked()
{
    is_drawimagineary = true;//描绘虚部
    is_drawmodel = false;
    is_drawreal =false;
}

void RealTimeSignal::on_radioButton_drawmodel_clicked()
{
    is_drawmodel = true;//描绘模
    is_drawreal =false;
    is_drawimagineary = false;
    //qDebug() <<is_realnumber<<is_sidebyside<<is_alternate<<is_drawmodel;
}

void RealTimeSignal::on_radioButton_sidebyside_clicked()
{
    is_sidebyside = true;//复数的存储形式为并列排列
    is_alternate = false;
    //ui->radioButton_alternate->setEnabled(false);
    ui->radioButton_drawreal->setEnabled(true);
    ui->radioButton_drawimagine->setEnabled(true);
    ui->radioButton_drawmodel->setEnabled(true);
}

void RealTimeSignal::on_radioButton_test_clicked()
{
    is_test = true;
    is_use = false;
}

void RealTimeSignal::on_radioButton_use_clicked()
{
    is_use = true;
    is_test = false;
}
//选择action槽函数
void RealTimeSignal::on_radioButton_choose_toggled(bool checked)
{
    QCustomPlot *customplot=ui->widget;
    if(checked)
    {
        //ui->radioButton_enlarge->setChecked(false);//取消放大
        ui->radioButton_drag->setChecked(false);//取消拖动选项
        customplot->setInteraction(QCP::iRangeDrag,false);//取消拖动
        customplot->setSelectionRectMode(QCP::SelectionRectMode::srmSelect);
        is_choose = true;
    }
    else
    {
        customplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    }
}
//放大action槽函数
//void RealTimeSignal::on_radioButton_enlarge_toggled(bool checked)
//{
//    QCustomPlot *customplot = ui->widget;
//    if(checked)
//    {
//        ui->radioButton_choose->setChecked(false);
//        ui->radioButton_drag->setChecked(false);
//        customplot->setInteraction(QCP::iRangeDrag,false);//取消拖动，拖动和放大不能同时使用
//        //customplot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
//        customplot->setInteraction(QCP::iRangeZoom);
//        is_choose = false;
//    }
//    else {
//        customplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
//    }


//}
//拖动action
void RealTimeSignal::on_radioButton_drag_toggled(bool checked)
{
    QCustomPlot *customplot = ui->widget;
    if(checked)
    {
        ui->radioButton_choose->setChecked(false);
        //ui->radioButton_enlarge->setChecked(false);
        customplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        customplot->setInteraction(QCP::iRangeDrag,true);
        is_choose = false;
    }
    else {
        customplot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
        customplot->setInteraction(QCP::iRangeDrag,false);//取消拖动

    }
}

void RealTimeSignal::receiveLeft(double left_num)
{
    if(is_choose)//只有在选择模式下才能设置数据的显示范围
    {
        qreal left = left_num;
        Left = qRound(left);
        qDebug() <<"左边界为："<< qRound(left);
    }
}
void RealTimeSignal::receiveRight(double right_num)
{
    if(is_choose)
    {
        qreal right = right_num;
        qDebug() <<"右边界为："<< qRound(right);
        Right = qRound(right);
    }
}

void RealTimeSignal::on_radioButton_realpart_clicked()
{
    is_realnumber = true;
    ui->radioButton_alternate->setEnabled(false);
    ui->radioButton_sidebyside->setEnabled(false);
}

//恢复模式
void RealTimeSignal::on_radioButton_toggled(bool checked)
{
    if(checked)
    {
        is_choose = false;
        QCustomPlot *customplot = ui->widget;
        this->Left = 0;
        this->Right = this->point_num;
        customplot->graph(0)->rescaleAxes();
        customplot->replot();
    }
}

void RealTimeSignal::on_radioButton_datasave_clicked()
{
    save_data = true;
}
//绘制曲线
void RealTimeSignal::drawgraph()
{
    QString data_type = "NULL";//记录用户目前想要显示的是实数、复数并列求模还是什么

    QCustomPlot *customplot = ui->widget;
    customplot->xAxis->setRange(0,this->point_num - 1);//设置显示的点数
    if(is_realnumber)
    {
        data_type = "real";//实数
    }
    else
    {
        if(is_alternate)
        {
            if(is_drawmodel)
            {
                data_type = "fualternate_model";//复数交替排列求模
            }
            else if(is_drawreal)
            {
                data_type = "fualternate_real";//复数交替排列实部
            }
            else if(is_drawimagineary)
            {
                data_type = "fualternate_imagineary";//复数交替排列虚部
            }
        }
        else if(is_sidebyside)
        {
            if(is_drawmodel)
            {
                data_type = "fusidebyside_model";//复数并列排列求模
            }
            else if(is_drawreal)
            {
                data_type = "fusidebyside_real";//复数并列排列实部
            }
            else if(is_drawimagineary)
            {
                data_type = "fusidebyside_imagineary";//复数并列排列虚部
            }
        }
    }
    if(data_type == "NULL")
    {
        qDebug() <<"绘制类型没有记录，请重新检查 ";
    }


    customplot->graph(0)->rescaleValueAxis();

}


void RealTimeSignal::closeEvent(QCloseEvent *event)
{
    //qDebug() << "窗口已关闭";
    emit isWindowClosed();
}
