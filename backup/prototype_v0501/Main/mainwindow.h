#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/**
* @projectName   prototype_v0719
* @brief         整个程序的主窗口：打开现有的dpsp工程时。
* @author        Antrn
* @date          2019-08-12
*/
#include "mainwindownewscene.h"
#include "menu_iteamoperation.h"
#include "opengl.h"
#include "page_structure/page_structure_utils.h"
#include <QCloseEvent>
#include <QMainWindow>

namespace Ui
{
class MainWindow;
}
/**
 * @brief The MainWindow class 总体仿真窗口 （点击打开）
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //shao：根据这个参数的值，判断是否需要保存当前窗口，其他的store管理的窗口最好也要有这个属性
     bool isSave=true;
    explicit MainWindow(QString title, QString dpsp_path, QWidget *parent = nullptr);
    ~MainWindow();
    void initOpenGL();
    // 加载工程文件
    bool loadProject(QString pro_path);
    // 关闭窗口事件
    void closeEvent(QCloseEvent *event);

    // Antrn: 保存当前列表中的设备 //shao:页面结构调整后复用这个保存函数，返回值改成bool类型
    bool saveStatus();
signals:
    void set_window(int a, int b, QWidget* Widget);

private slots:

    //向子类发送itemoperate，从mainwindownew粘贴过来的,id就是设备名字,
    void itemOperateSlot(Menu_iteamOperation::OperateType, Page_structure_utils::window_type list_type, QString id);

    void on_actionOpenGL_triggered();
    void on_actionexit_triggered();
    void on_actionCurve_triggered();
    void on_actionToolsBar_triggered();
    void on_actionEquip_triggered();

    void on_actiondataPlayBack_triggered();

    void on_actionedit_triggered();

    void on_actionsaveproject_triggered();

    void on_actionReset_triggered();

    void on_actionCenter_triggered();

    void on_actionZoomin_triggered();

    void on_actionZoomout_triggered();

    void on_actiondelete_triggered();

private:
    Ui::MainWindow *ui;
    //为了在itemOperateSlot中可以向graphicsScene中的控件传递信息
    MainWindowNewScene *graphicsScene;
    OpenGL *openGL;
    // 窗口名称
    QString w_title;
    QString p_dpsp_path;
    // 工程的配置文件信息
    QMap<QString, QString> info;
    // 工程中包含的其他元素，将其id,name保存
    QMap<QString, QString> comps;
    void readSettings();
    void writeSettings();
     // shao:新建工程文件夹
    QString new_a_prohect(QString title);
};

#endif // MAINWINDOW_H
