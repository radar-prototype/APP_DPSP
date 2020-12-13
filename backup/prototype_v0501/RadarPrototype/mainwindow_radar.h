#ifndef MAINWINDOW_RADAR_H
#define MAINWINDOW_RADAR_H
#include <QAbstractButton>
#include <QComboBox>
#include <QFontComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QToolBox>
#include <QToolButton>
#include <QListWidget>
#include <QPushButton>
#include <QUndoCommand>
#include <QUndoView>
#include <sysmessage.h>
#include "diagramitem.h"
#include "radarscene.h"
#include "diagramitem.h"
#include "diagramtextitem.h"
#include "leftnavi.h"
#include "comp_cpu.h"
#include "comp_thread.h"
#include "RadarPrototype/development_panel/code_generate/main_process.h"

/**
* @projectName   prototype_v0719
* @brief         编辑雷达功能的主窗口。
* @author        Antrn
* @date          2019-08-12
*/
namespace Ui
{
class MainWindow_Radar;
}
class MainWindow_Radar : public QMainWindow
{
    Q_OBJECT

public:
//    explicit MainWindow_Radar(QWidget *parent = nullptr);
    explicit MainWindow_Radar( QString id, QWidget *parent = nullptr);
    ~MainWindow_Radar();
    DiagramItem *getDiagramItemById(QString item_id);
    void deleteItemArrowById(QString id);
    void deleteArrowById(QString id);
    void autoloadXmlById(QString id);
    QFileInfoList getFileList(QString path);
    QString getEquip_id() const;
    void setEquip_id(const QString &value);
    void readXmlConf(QString xmlname);
    // tip：true，反馈保存成功与否的信息
    bool  save2XmlFile(bool tip = true);//shao:22-11-18修改了返回值
    //默认程序一打开就是已经保存好的状态
//    static bool isSave;
     bool isSave=true;
    void toggleSaveXml(int flag);
    // 初始化5个算法组件
    void init5Comp();
//    void init1Comp(QString comPName, QMenu *itemMenu, DiagramItem::DiagramType diagramType);
    // Antrn: 初始化一个组件
    void init1Comp(QString comPName, QMenu *itemMenu, QString iconName);
    // Antrn: 按照名字加载组件
    void loadCompByName(QString strText);
    // Antrn: 显示属性悬浮框的函数
    void toShowPropertiesDock(AlgorithmComp ac, bool isReadonly = true);
    //标记输入输出组件是否存在场景中
    void itemMarked(QString DName);
    // Antrn: 将场景中框选中的所有对象变为一个大组件
    QString makeSelectedItems2OneComp(QString new_name, QString cnName, QString callFunc);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void on_actionCom_list_triggered();
    void xy_show(double x, double y);

    // 读取文件夹，加载所有组件
    void loadAllComps();

    void backgroundButtonGroupClicked(QAbstractButton *button);
//     void buttonGroupClicked(int id);
    void deleteItem();
    void copyItem();
    void addItem2Lib();
    void showThisWave();

    void pointerGroupClicked(int id);
    void bringToFront();
    void sendToBack();
    void showItemProperties();
    void textInserted(QGraphicsTextItem *item);
    void currentFontChanged(const QFont &font);
    void fontSizeChanged(const QString &size);
    void sceneScaleChanged(const QString &scale);
    void textColorChanged();
    void itemColorChanged();
    void lineColorChanged();
    void textButtonTriggered();
    void fillButtonTriggered();
    void lineButtonTriggered();
    void libButtonTriggered();
    void handleFontChange();
    void itemSelected(QGraphicsItem *item);
    void on_actionOpenXml_triggered();

    void on_actionsave_triggered();

    void On_start();
    void On_over();
    void On_rateSignal(float rate);

    void On_isSave2False(QString message);

    // 更新右面的雷达组件的属性
    void update_Comp_property(AlgorithmComp ac);

//     void setComp_typeandMode(int id);
    void setComp_typeandMode(QString iconName, RadarScene::Mode mode);
    void search();
    void editComplete();

    void on_tabWidget_2_destroyed();

    void on_tabWidget_2_tabBarClicked(int index);

    void on_tabWidget_2_tabCloseRequested(int index);
    void on_actiongene_triggered();

    void on_actionGeneCode_triggered();

    void on_action_run_triggered();

    bool on_action_sendCode_triggered();

    void on_action_rand_add_triggered();

    void on_action_compile_triggered();

    void on_actionproperty_toggled(bool arg1);

    void on_actionmanage_Node_triggered();

    void on_actionClusterState_triggered();

    //点击箭头右键触发
    void deal_Arrowdelete(QString ID);
    //shao:单个组件参数配置
    void comp_para_config();
    // 选择算法组件使用的CPU
    void chooseCPU();

    // 选择算法组件使用的线程
    void chooseThread();

    void addAndsortDiagramList(QList<DiagramItem *> *diagramlist, QList<QGraphicsItem *> list);


public slots:
    void itemInserted(DiagramItem *item);
    // 接收信息
    void receiveFromSend(QString message, int flag);

    // 接收场景中的组件的id信息，传给mainwindow_radar展示属性
    void receiveItemsid2showProperties(QString sid);

    // 为雷达组件生成icon图标
    void generateIcon(QString icon_name);

    // 编辑场景中的算法组件时候的槽函数
    void codeEditSlot();


    //接收场景中的组件信息，传给队列信息表展示
    void receiveIteminformation(QVariant variant);

    //接收用户输入的线程数量，设置组件的线程数量属性
    void deal_threadnum(int num);

    //接收用户选择的节点IP，设置在组建的IP地址属性
    void deal_IPaddress(QString ip_address);

    //接收信号显示页面的关闭信号，设置isSignalShowOpen
    void deal_isSignalShowOpen();
public:
    //代码生成
    // 头文件引入模板
    QString code_template_start = "#include \"RspMsgQueue.h\"\n";

    QString declareMid = "";

    QString Simulate_Component = "void *Simulate_Component(void *arg)\n"
                                 "{\n\twhile (1)\n\t{\n\t\tchar *p = \"aaaaaaaaaaaaa\";\n"
                                 "\t\tmsgQSendNext(&gMsg[_AFTER_FIRST_]DatQ, p, 15, 0, 1, &param_sim, &param_dpc);\n\t\tusleep(200);\n\t}\n}\n";
//COMPONENT_NAME 组件全名 GMSGQ_LAST上一个消息队列的名字
    QString fun_component = "void *[_COMPONENT_NAME_]_Component(void *arg)\n"
                            "{\n\twhile (1)\n\t{\n\t\t"
                            "msgQReceivePre(&gMsg[_GMSGQ_LAST_]DatQ, p, 15, 0, &param_[_LOWERCASE_LAST_], &param_[_LOWERCASE_THIS_]);\n"
                            "\t\tmsgQSend(&gMsg[_GMSGQ_THIS_]Q, p, 15, 0, 1);\n"
                            "\t\tfree(p);\n\t}\n}\n";

    QString fun_thread = "void *[_COMPONENT_NAME_]_thread(void *arg)\n"
                         "{\n\twhile (1)\n\t{\n\t\tchar *p = (char *)malloc(15 * sizeof(char));\n"
                         "\t\tmsgQReceive(&gMsg[_GMSGQ_THIS_]Q, p, 15, 0);\n"
                         "\t\tmsgQSendNext(&gMsg[_GMSGQ_THIS_]DatQ, p, 15, 0, 1, &param_[_LOWERCASE_THIS_], &param_[_LOWERCASE_NEXT_]);\n"
                         "\t\tfree(p);\n\t}\n\treturn NULL;\n}\n";

    QString main_params_init = "\nint main(int arg, char *argv[]){\n"
                               "\tint i, thread_id[_THREAD_ALL_];\n";

    QString code_template_end = "}\n";

    bool getIsDisplayExist() const;
    void setIsDisplayExist(bool value);

private:
    //shao:新的代码生成 模块引入
    Main_process* main_process;
    // Antrn: 控制台按钮
    QPushButton *openConsole;
    // mao: 雷达名字
    QString equip_id;
    Ui::MainWindow_Radar *ui;
    // Antrn: 显示xy坐标的label，在屏幕左下角
    QLabel *label_xy;
    // Antrn: 组件列表搜索框
    QLineEdit *m_pSearchLineEdit;
    // Antrn: 是否已经选择过保存场景的路径
    bool isSelectPath = false;
    // Antrn: 用户选择的保存路径文本
    QString userSelectPath;
    // Antrn: 候选单词列表
    QStringList word_list;
    QStringListModel *string_list_model;
    // Antrn: 场景指针
    RadarScene *scene;

    QMenu *fileMenu;
    QMenu *itemMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
    QComboBox *textColorCombo;
    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;

//    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup;

// 侧边按钮
    QPushButton *sideButton;
// 左边导航栏
    LeftNavi *lft;
    // Antrn: DEL
//    void createToolBarNull();

    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    // Antrn: 打包进组件库
    QToolButton *save2LibButton;

    QLabel *label_time;
    QProgressBar *progressBar;

    void createCompBox();
    void createActions();
    void createMenus();
    void createToolbars();

    QWidget *createBackgroundCellWidget(const QString &text,
                                        const QString &image);
//    QWidget *createCellWidget(const QString &text, DiagramItem::DiagramType type);
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);
    void saveSnapshot(int flag);

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;
//    QAction *undoAction;
//    QAction *redoAction;
    QAction *propertyAction;
    QAction *codeEditAction;

    QAction *toFrontAction;
    QAction *sendBackAction;
// 复制场景中的组件
    QAction *copyAction;
// 加入组件库
    QAction *add2CompLibraryAction;
// 展示波形图
    QAction *showWaveGraphicAction;
    //shao:组件参数配置
    QAction *para_config_action;
//选择CPU和线程
    QAction *chooseCPU_Action;
    QAction *chooseThread_Action;

    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;

    QDockWidget *otherDoc;
    //代表输入输出显示组件是否存在
    bool isInputExist = false;
    bool isOutputExist = false;
    bool isDisplayExist = false;
    //代表是否已经打开信号显示窗口
    bool isSignalShowOpen = false;

//组件使用的CPU选择窗口
    comp_cpu *cpu_dialog;
//组件使用的线程选择窗口
    comp_thread *thread_dialog;

//    QUndoStack *undoStack;
//    QUndoView *undoView;

    SysMessage *msg;

//看到代码这么多，我还要加，正的于心不忍
signals:
//    返回自己(其实暂时只用了id)
    void iClose(MainWindow_Radar*);
// 接收要打印到程序输出的语句，flag：0红色，1黑色
    void send2AppOutput(QString message, int flag);
    void createProjectFiles();
private slots:
    void on_action_config_triggered();

};
#endif // MAINWINDOW_RADAR_H
