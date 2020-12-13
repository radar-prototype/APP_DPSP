#ifndef REALTIMESIGNAL_H
#define REALTIMESIGNAL_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "receivethread.h"
#include <QTime>
#include <QtGlobal>
#include <QTimer>
#include "worker.h"
#include <QThread>
#include <QMetaType>
#include <QCloseEvent>

extern "C"{
#include "RspMsgDDS.h"
}
namespace Ui {
class RealTimeSignal;
}

class RealTimeSignal : public QMainWindow
{
    Q_OBJECT

public:
    explicit RealTimeSignal(QWidget *parent = nullptr);
    ~RealTimeSignal();
    //weishi:记录点的数量
    //int num = 0;
    //用界面的参数初始化速度点数等信息
    void setupinfo();


private slots:
    //
    void on_pushButton_run_clicked();

    void on_lineEdit_speed_returnPressed();

    void on_pushButton_pause_clicked();

    void on_lineEdit_returnPressed();

    void on_comboBox_activated(const QString &arg1);

    void on_lineEdit_offset_returnPressed();

    void on_radioButton_imaginarypart_clicked();

    void on_radioButton_alternate_clicked();

    void on_radioButton_drawreal_clicked();

    void on_radioButton_drawimagine_clicked();

    void on_radioButton_drawmodel_clicked();

    void on_radioButton_sidebyside_clicked();

    void on_radioButton_test_clicked();

    void on_radioButton_use_clicked();

    void on_radioButton_choose_toggled(bool checked);
//    放大按键槽函数
//    void on_radioButton_enlarge_toggled(bool checked);

    void on_radioButton_drag_toggled(bool checked);

    //void on_customplot_selectionChangedByUser();

    void on_radioButton_realpart_clicked();

    void on_pushButton_stop_clicked();

    //void contextMenuRequest(QPoint pos);

    //void on_RealTimeSignal_customContextMenuRequested(const QPoint &pos);

    void on_radioButton_toggled(bool checked);

    void on_radioButton_datasave_clicked();


signals:
    /*
     * 用来触发新线程的计算函数，传递给计算子线程的
     * 有需要计算的数组，数组的大小,数组的数据类型
     * 用户想要显示的数据类型，负数并列排列求模时的偏移量,以及是否存储数据
     */
    void startRunning(char *, int, QString, QString, int, bool);
    //窗口关闭时发送信号，此时信号页面才可以再次打开。目的是防止同一个显示组件多次打开信号显示页面
    void isWindowClosed();

public slots:
    //用户点击运行时触发，完成图的初始化
    void setupRealtimeDataDemo();
    //绘图，根据定时器更新图表
    void drawgraph();
    //鼠标悬浮事件槽函数
    void myMoveEvent(QMouseEvent *event);
    //接收qcustomplot传送过来的左右边界值
    void receiveLeft(double left_num);
    void receiveRight(double right_num);

    //weishi:接收线程的处理结果
    void receive_result(QVector<double> res);

    void handleData(char*add, int len);
    //调整信号显示页面的显示范围
    //void rescaleAxes();
    //右键点击
    //void contextMenuEvent ( QContextMenuEvent * event );

private:
    char* buffer;

    Ui::RealTimeSignal *ui;

    //测试用：
    QString str;
    //计时器
    QTimer timer;
    //存储字符串转化后的数据
    QList<int> x;
    QList<double> y;
    //当通信中间件发送N组数据才选择1组数据进行绘制
    double  time_space = 1;
    //每次显示的点的数量
    int point_num = 2048;
//    int point_num = 10;
    //记录解析的数据类型
    QString data_type;
    //数据解析时的偏移量
    int offset = 304;
    //记录框选放大以后的显示范围
    int Left = 0;
    int Right;
//    int offset = 0;
    //是否描绘实部
    bool is_drawreal = false;
    //是否描绘虚部
    bool is_drawimagineary = false;
    //是否描绘模
    bool is_drawmodel = false;
    //接收数据是否为实数
    bool is_realnumber = false;
    //复数的排列方式是否为交替排列
    bool is_alternate = false;
    //复数的排列方式是否为并列排列
    bool is_sidebyside= false;
    //测试模式
    bool is_test = false;
    //实际使用模式
    bool is_use = true;
    //选择模式
    bool is_choose = false;
    //暂停时修改
    bool flag_canDrawGraph = true;
    //是否存储数据
    bool save_data = false;

    QThread myWorkThread;
    worker *myworker;//将中间件传送的数据转化成数组,求模后的小数组传到绘图界面

    ReceiveThread *rt;//接收中间件传送的数据

//    QVector<double> y1_result;//接收处理完的小数组
protected:
    //窗口关闭
    void closeEvent(QCloseEvent *event);
//    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
};

#endif // REALTIMESIGNAL_H
