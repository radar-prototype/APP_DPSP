#ifndef MAINWINDOWNEW_H
#define MAINWINDOWNEW_H
/**
* @projectName   prototype_v0719
* @brief         新建工程时的主窗口。（shao:现在这个）
* @author        Antrn
* @date          2019-08-12
*/
#include "mainwindownewscene.h"
#include <QMainWindow>
#include <QLabel>
#include <mainwindow_radar.h>
// 仅仅为了引入枚举值OperateType
#include "menu_iteamoperation.h"

namespace Ui {
class MainWindowNew;
}
/**
 * @brief The MainWindowNew class 总体仿真窗口（点击新建）
 */
class MainWindowNew : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindowNew(QWidget *parent = nullptr);
    ~MainWindowNew();
    QLabel *label_xy;
//    MainWindowNewScene *graphicsScene;
//    static QList<MainWindow_Radar *> main_radar_list;
    bool saveListContent2pro();

signals:
//接收子类itemoperate
    void itemOperate(Menu_iteamOperation::OperateType, QString );
    void set_window(int a,int b, QWidget* Widget);
public slots:
    //向子类发送itemoperate,type:object/radar/ecm,name:设备名称
    void itemOperateSlot(Menu_iteamOperation::OperateType operateType, QString type, QString name, QString newName="");
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void xy_show(double x,double y);

    void on_actionabout_DPSP_triggered();

    void on_actionexit_triggered();

    void on_actio_leftDock_triggered();

    void on_actionProperty_triggered(bool checked);

    void on_actionsaveFile_triggered();

    void on_actionnewFile_triggered();

private:
    Ui::MainWindowNew *ui;
    QRect rect;
    //为了在itemOperateSlot中可以向graphicsScene中的控件传递信息
    MainWindowNewScene *graphicsScene;
    //属性窗口内容显示
    void propertyContent(QString);
    // 工程的配置文件信息
    QMap<QString, QString> infos;
    // 工程中包含的其他元素，将其id,name保存
    QMap<QString, QString> comps;
};

#endif // MAINWINDOWNEW_H
