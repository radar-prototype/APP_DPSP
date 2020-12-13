#include "mainwindow_radar.h"
#include "radarscene.h"
#include "ui_mainwindow_radar.h"
#include <QDebug>
#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QToolBar>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QLineEdit>
#include <QPushButton>
#include <utils.h>
#include <QCompleter>
#include <mainwindownew.h>
#include <QDesktopWidget>
#include "arrow.h"
#include "compproperty.h"
#include "clickablelabel.h"
#include <QFormLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProgressDialog>
#include <QSizePolicy>
#include <algocodeedit.h>
#include <codepreview.h>
#include <codewindow.h>
#include <qudong.h>
#include <realtimecurve.h>
#include <runcompconf.h>
#include <simdatagenwidget.h>
#include "leftnavi.h"
#include "sysmessage.h"
#include "clusterstatedialog.h"
extern "C" {
#include "requestMsg.h"
}
#include <development_panel/signalshow/realtimesignal.h>
#include "development_panel/comp_encapsulation/infoinputdialog.h"

//const int InsertTextButton = 10;
// This is an auto-generated comment by Antrn.
/************************************
 * @projectName   prototype_v0719
 * @author Antrn
 * @date 2019-08-12 T 14:35:51
 * @description 编辑雷达功能的主窗口
 * @copyright (c)
 ************************************/
//bool MainWindow_Radar::isSave = true;
MainWindow_Radar::MainWindow_Radar(QString id, QWidget *parent) :
    QMainWindow(parent),
    equip_id(id),
    ui(new Ui::MainWindow_Radar)
{
    ui->setupUi(this);
    // FIXME 加上日志系统关闭雷达工程时候就会报错heap溢出，不知道为什么
    // HEAP[prototype_v0501.exe]:
    // Heap block at 28C768F0 modified at 28C769F8 past requested size of 100
    // 20200730 修复，将256的buf_tmp改为1024，将传入的textEdit进行一个null的判断即可
//    SysMessage::open_sys_message(ui->textEdit);
    // 去掉ui->textEdit
//    SysMessage::open_sys_message_without_textEdit();
//    qInstallMessageHandler(SysMessage::SysMessageOutput);

    //shao:代码生成组件
    this->main_process = new Main_process(this);

    this->setAttribute(Qt::WA_DeleteOnClose); //关闭销毁变量
//    lft = new LeftNavi();
//    lft->setFrameStyle(QFrame::NoFrame);

    // Antrn: 最大窗口   shao:不去掉会有切换会白场，去掉会出现奇怪的窗口，迷惑
//    this->showMaximized();

    // equip_id就是雷达的名字，作为唯一标识
    setWindowTitle(tr((this->equip_id).toUtf8().data()));
    // 设置这个，场景视图的位置会发生变化，坐标从0变为中点2500
    // setCentralWidget(ui->graphicsView);
    setCentralWidget(ui->splitter);
    //兼容性？？
    setUnifiedTitleAndToolBarOnMac(true);

    connect(this, &MainWindow_Radar::createProjectFiles, [ = ]()
    {
        bool tof = Utils::createProject(getEquip_id());
        qDebug() << "工程文件是否创建成功：" << tof;
    });

    emit createProjectFiles();

    // 合并dock窗口
    otherDoc = new QDockWidget(this);
    otherDoc->setWindowTitle(tr("其他窗口"));
    otherDoc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
    otherDoc->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::LeftDockWidgetArea, otherDoc);
    tabifyDockWidget(otherDoc, ui->dockCompList);

    // 想要隐藏标题栏，没用
    // ui->dockWidget_3->setWindowFlags(Qt::WindowTitleHint|Qt::CustomizeWindowHint);
    QWidget* titleBar = ui->dockWidget_3->titleBarWidget();
    QWidget* emptyWidget = new QWidget();
    ui->dockWidget_3->setTitleBarWidget(emptyWidget);
    delete titleBar;
    // 默认关闭
    ui->dockWidget_3->close();
    // 默认显示组件列表
    ui->dockCompList->raise();

    //组件列表设置(左边的窗口)
    ui->actionCom_list->setCheckable(true);
    ui->actionCom_list->setChecked(true);

    createActions();
    createCompBox();
    createMenus();
    createToolbars();

    scene = new RadarScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(itemSelected(QGraphicsItem*)));

    //点击run菜单项（或工具栏图标）触发
    connect(ui->actionRunRadar, SIGNAL(triggered()),
            scene, SLOT(startRunCode()));
//    connect(ui->action_Stop,SIGNAL(triggered()),graphicsScene,SLOT(stopSimulation()));
    connect(scene, SIGNAL(startRun()),
            this, SLOT(On_start()));
    connect(scene, SIGNAL(overRun()),
            this, SLOT(On_over()));
    connect(scene, SIGNAL(rateSignal(float)),
            this, SLOT(On_rateSignal(float)));
    // 场景控制属性显示
    connect(scene, &RadarScene::refreshPropertyDock, this, &MainWindow_Radar::toShowPropertiesDock);

    ui->graphicsView->setScene(scene);

    // xy坐标标签
    label_xy = new QLabel(this);
    label_xy->setFixedWidth(100); //for testing
    label_xy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    label_xy->setStyleSheet("QLabel{margin-left:10px;color:white}QLabel:hover{color:#00AA00;}");
    ui->statusbar->setStyleSheet(QString("QStatusBar::item{border: 0px}"));
    // 状态栏左下角加入xy坐标
    ui->statusbar->addWidget(label_xy);
    // 设置鼠标跟踪开启
    ui->graphicsView->setMouseTracking(true);

    ui->graphicsView->setAcceptDrops(true);
    //设置可以使鼠标在视图上拖拽出橡皮筋框选择多个组件
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    connect(scene, SIGNAL(signal_xy(double, double)), this, SLOT(xy_show(double, double)));

    connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)),
            this, SLOT(textInserted(QGraphicsTextItem*)));

    // 关联是否保存xml的按钮可用的信号和槽函数
    connect(scene, &RadarScene::isSave2False, this, &MainWindow_Radar::On_isSave2False);

    //点击箭头右键删除键触发
    connect(scene, &RadarScene::delete_arrowSignal, this, &MainWindow_Radar::deal_Arrowdelete);

    //打开自动读取已有的rad文件
    autoloadXmlById(getEquip_id());
    //    QGridLayout *layout = new QGridLayout;
//    layout->addWidget(ui->dockCompList);

//    layout->addWidget(ui->graphicsView);

    //抓手
    //ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag );

//    QWidget *widget = new QWidget;
//    widget->setLayout(layout);

//    setCentralWidget(widget);
    connect(ui->listWidget, &RadarCompDraglistWidget::add_one_Comp, this, &MainWindow_Radar::update_Comp_property);
    connect(ui->listWidget, &RadarCompDraglistWidget::toRefreshCompList, this, &MainWindow_Radar::loadAllComps);
//    u = new Utils;
    // 当新增组件时候，设置插入模式和插入的组件的类型
    connect(ui->listWidget, &RadarCompDraglistWidget::setComp_typeandMode, this, &MainWindow_Radar::setComp_typeandMode);

//    第一次设置setChecked不会触发槽函数，所以直接调用了
//    ui->actionproperty->setChecked(false);
    //关闭属性窗口
    on_actionproperty_toggled(false);
    // 属性dock窗口
//    ui->dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);

    // Antrn: 默认打开第二个tab
    ui->tabWidget->setCurrentIndex(1);
    // 打印信息
    connect(this, &MainWindow_Radar::send2AppOutput, this, &MainWindow_Radar::receiveFromSend);
    connect(ui->listWidget, &RadarCompDraglistWidget::sendMessage, this, &MainWindow_Radar::receiveFromSend);
    connect(ui->listWidget, &RadarCompDraglistWidget::send_icon_name, this, &MainWindow_Radar::generateIcon);


    openConsole = new QPushButton;
    openConsole->setText(tr("控制台"));
    openConsole->setObjectName("ptn_oc");
    openConsole->setFixedWidth(60);
    ui->statusbar->addWidget(openConsole);
    // 下面控制console显示隐藏
    connect(openConsole, &QPushButton::clicked, ui->dockWidget_3, [ = ]()
    {
        if(ui->dockWidget_3->isVisible())
        {
            ui->dockWidget_3->close();
            openConsole->setStyleSheet("");
        }
        else
        {
            ui->dockWidget_3->show();
            openConsole->setStyleSheet("#ptn_oc{"
                                       "background-color:rgb(160,160,160);"
                                       "color:rgb(250,250,250);"
                                       "border-style:outset;"
                                       "border-color:rgb(0,0,0);"
                                       "}");
        }
    });

    // 在文件夹中添加算法组件 //receiveAlgo4listWidget 插入组件的第一步
    connect(ui->listWidget, &RadarCompDraglistWidget::addAlgo2Scene, scene, &RadarScene::receiveAlgo4listWidget);

    // 端口流量监控
    QFont font = ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget->horizontalHeader()->setFont(font);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setFrameShape(QFrame::Panel);
    ui->tableWidget->setFrameStyle(QFrame::Plain);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#eee;"); //设置选中背景色
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightgray;border:1px solid #8faac9;color: black;font:bold 14px '微软雅黑';text-align:center;height:32px;}"); //设置表头背景色
    // Antrn: DEL 侧边缩进栏，半成品，待删，注意全局搜索： sideButton
#if 0
    QSplitter *mainSplitter = new QSplitter(this);
    mainSplitter->setFrameStyle(QFrame::NoFrame);
    mainSplitter->setHandleWidth(1);
    mainSplitter->setStyleSheet(
        QString("QSplitter::handle {background: qlineargradient("
                "x1: 0, y1: 0, x2: 0, y2: 1,"
                "stop: 0 %1, stop: 0.07 %2);}").
        arg(qApp->palette().background().color().name()).
        arg(qApp->palette().color(QPalette::Dark).name()));

    mainSplitter->setChildrenCollapsible(false);

    mainSplitter->addWidget(lft);
    mainSplitter->addWidget(ui->dockWidget);
    mainSplitter->addWidget(ui->graphicsView);
    mainSplitter->addWidget(ui->dockCompList);
    mainSplitter->setStretchFactor(20, 80);
    createToolBarNull();
    QHBoxLayout *ptrHLayout = new QHBoxLayout();
    ptrHLayout->setMargin(0);
    ptrHLayout->setSpacing(0);
    ptrHLayout->addWidget(sideButton);
    ptrHLayout->addWidget(mainSplitter);

    QVBoxLayout *ptrVLayout = new QVBoxLayout();//创建布局
    ptrVLayout->addLayout(ptrHLayout);
    ptrVLayout->setSpacing(0);  //设置间距与边缘空白
    setLayout(ptrVLayout);
#endif
}

// Antrn: DEL 跟上面的代码一起删除
#if 0
void MainWindow_Radar::createToolBarNull()
{
    sideButton = new QPushButton(this);
    sideButton->setObjectName("pushButtonNull");
    sideButton->setIcon(QIcon(":/image/triangleR.png"));
    sideButton->setFixedWidth(10);
    sideButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sideButton->setFocusPolicy(Qt::NoFocus);
    sideButton->setStyleSheet("background: #E8E8E8; border: none; padding: 0px;");
}
#endif

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-10-15 T 15:45:01
 * @copyright (c)
 * @status Debug
 * @brief 场景缩放比例变化事件，处理
 * @param scale
 */
void MainWindow_Radar::sceneScaleChanged(const QString &scale)
{
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "xml由于场景缩放比例变化而改变";
//    emit send2AppOutput("xml由于场景缩放比例变化而改变",1);
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = ui->graphicsView->matrix();
    ui->graphicsView->resetMatrix();
    ui->graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
    ui->graphicsView->scale(newScale, newScale);
}

// 点击按钮选择模式
void MainWindow_Radar::pointerGroupClicked(int )
{
    scene->setMode(RadarScene::Mode(pointerTypeGroup->checkedId()));
}

//scene槽函数
void MainWindow_Radar::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
        qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}

// 设置save按钮的可用与否
void MainWindow_Radar::toggleSaveXml(int flag)
{
    if(flag == 1)
    {
        ui->actionsave->setEnabled(true);
    }
    if(flag == 0)
    {
        ui->actionsave->setEnabled(false);
    }
}
#if 0
// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介 初始化5个默认的算法组件
* @author        Antrn
* @date          2019-10-03
*/
void MainWindow_Radar::init5Comp()
{
    init1Comp(tr("脉冲压缩"), itemMenu, DiagramItem::Comp1);
    init1Comp(tr("恒虚警率"), itemMenu, DiagramItem::Comp2);
    init1Comp(tr("输入"), itemMenu, DiagramItem::Comp3);
    init1Comp(tr("动目标检测"), itemMenu, DiagramItem::Comp4);
    init1Comp(tr("输出"), itemMenu, DiagramItem::Comp5);
}

// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介 初始化单个默认组件
* @author        Antrn
* @date          2019-10-03
*/
//void MainWindow_Radar::init1Comp(QString comPName, QMenu *itemMenu, DiagramItem::DiagramType diagramType)
void MainWindow_Radar::init1Comp(QString comPName, QMenu *itemMenu, QString iconName)
{
    QListWidgetItem *item = new QListWidgetItem();
    DiagramItem ditem(iconName, itemMenu);
    QIcon icon5(ditem.image());
    item->setIcon(icon5);
    item->setText(tr(comPName.toUtf8().data()));
    //这里的用户角色存储用户数据
    item->setData(Qt::UserRole, QPixmap(ditem.image()));
    item->setData(Qt::UserRole + 1, comPName);
    item->setData(Qt::UserRole + 2, ui->listWidget->count());
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
    ui->listWidget->addDragItem(item);
}
#endif


/**
* @projectName   prototype_v0906
* @brief         简介 根据用户搜索名字找到组件
* @author        Antrn
* @date          2019-09-27
*/
void MainWindow_Radar::loadCompByName(QString strText)
{
    QString m_sProjectPath = QDir::currentPath() + "/algoXml/"; /*文件夹全路径名*/
    QDir dir(m_sProjectPath);
    /*判断文件夹是否存在*/
    if (!dir.exists())
    {
        Utils::alert(QApplication::desktop()->screen()->rect(), tr("读取文件夹出错!"));
        return;
    }
    dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
    QStringList filterList;
    filterList << "*.xml";
    // 遍历查找
    foreach(QFileInfo fileInfo, dir.entryInfoList(filterList))
    {
        QString absolute_file_path = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.baseName();/*获取文件后名(不带后缀的文件名)*/
        if(fileName == strText)
        {
            AlgorithmComp ac = Utils::readPluginXmlFile(m_sProjectPath + fileName + ".xml");
            QString id = ac.getInfo()["ID"];
            QListWidgetItem *item0 = new QListWidgetItem();
            item0->setIcon(QIcon(":/img/component.png"));
            item0->setText(tr(fileName.toUtf8().data()));
            //这里的用户角色存储用户数据
            item0->setData(Qt::UserRole, QPixmap(":/img/component.png"));
            item0->setData(Qt::UserRole + 1, fileName);
            item0->setData(Qt::UserRole + 2, id);
            item0->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
            ui->listWidget->addDragItem(item0);
            break;
        }
    }
}
//属性窗口
/**
 * @brief 展示右边的属性Dock
 * @param ac 要解析展示的算法
 * @param isReadonly 是不是可以编辑的
 */
void MainWindow_Radar::toShowPropertiesDock(AlgorithmComp ac, bool isReadonly)
{
    // 展示出属性dock
    if(!ui->actionproperty->isChecked())
    {
        ui->actionproperty->setChecked(true);
    }
    ui->dockWidget->create(ac, isReadonly, scene, getEquip_id());
    //接受属性更新信号
//    2020/6/4 shao
//    感觉应该搞一个信号处理传回的 已经更新的组件 还要使得当前场景可以保存，但是没实现 以后有机会再看看在哪
//     connect(ui->dockWidget,&mainwindow_radar_property::compUpdata, [&]( AlgorithmComp *ac2 ){
//             // 先把scene的map中原来的一项删除
//             scene->deleteScene_comps(ac2->getInfo()["ID"]);
//             // 重新插入新的算法，id不变
//             scene->add2Scene_comps(ac2->getInfo()["ID"], *ac2);
//             // 重新写入文件
//             Utils::writeAlgorithmComp2Xml(*ac2, "/radar/"+getEquip_id()+"/room");
//             // 刷新一下属性Dock，默认不能编辑
//             update_Comp_property(*ac2);
//     });
}


// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介 加载所有组件， 目前只是加载了默认的五个组件
* @author        Antrn
* @date          2019-09-27
*/
void MainWindow_Radar::loadAllComps()
{
    ui->listWidget->clear();
    // 加入新建雷达按钮项
    ui->listWidget->addItem(tr("自定义组件"));
    ui->listWidget->addCompButton = ui->listWidget->item(0);
    ui->listWidget->addCompButton->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->listWidget->addCompButton->setFlags(Qt::NoItemFlags);
    ui->listWidget->addCompButton->setIcon(QIcon(":/img/newradar.png"));
    ui->listWidget->addCompButton->setToolTip(tr("点击增加自定义组件"));
    // 换成读取组件文件夹里的所有组件
//    init5Comp();

    // 读取文件夹下所有的文件
    QString m_sProjectPath = QDir::currentPath() + "/algoXml/"; /*文件夹全路径名*/
    QDir dir = Utils::openDirOrCreate(m_sProjectPath);
    dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
    QStringList filterList;
    filterList << "*.xml";
    // for循环读取有点慢
    foreach(QFileInfo fileInfo, dir.entryInfoList(filterList))
    {
        QString absolute_file_path = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.baseName();/*获取文件后名(不带后缀的文件名)*/
        if(!ui->listWidget->nameList.contains(fileName))
        {
            ui->listWidget->nameList.append(fileName);
            qDebug() << "mainwindow_radar加入组件名:" << fileName;
//            emit send2AppOutput(QStringLiteral("加入文件名: ")+fileName,1);
        }
        else
        {
            qDebug() << "加载组件时有重复的文件名存在，文件名： " << fileName;
            emit send2AppOutput(QStringLiteral("[警告]： 加载组件时有重复的文件名存在，文件名： ") + fileName, 0);
        }
        AlgorithmComp ac = Utils::readPluginXmlFile(m_sProjectPath + fileName + ".xml");
        ui->listWidget->algorithms.insert(ac.getInfo()["ID"], ac);

        QListWidgetItem *item0 = new QListWidgetItem();
        item0->setIcon(QIcon(":/img/component.png"));
        item0->setText(tr(fileName.toUtf8().data()));
        //这里的用户角色存储用户数据
        item0->setData(Qt::UserRole, QPixmap(":/img/component.png"));
        item0->setData(Qt::UserRole + 1, fileName);
        // 这个id不再是数字，而是uuid
        item0->setData(Qt::UserRole + 2, ac.getInfo()["ID"]);
        item0->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
        ui->listWidget->addDragItem(item0);
    }
}
/**
* @brief 简介 当放入场景中的组件为输入输出组件时进行标记
*/
void MainWindow_Radar::itemMarked(QString DName)
{
    if(DName == "INPUT")
    {
        isInputExist = true;
    }
    else if(DName == "OUTPUT")
    {
        isOutputExist = true;
    }
    else if(DName.compare("display") == 0)
    {
        isDisplayExist = true;
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-09-28
 * @brief MainWindow_Radar::makeSelectedItems2OneComp
 * @return 是否打包成功的返回信息
 */
QString MainWindow_Radar::makeSelectedItems2OneComp(QString new_name, QString cnName, QString callFunc)
{
    // Antrn: 获取场景中选中的组件列表
    QList<QGraphicsItem*> l = this->scene->selectedItems();
    // Antrn: 送到函数里排序，获得不包括箭头的算法组件
    QList<DiagramItem*> dl;
    // Antrn: 2个组件一个箭头，否则没必要打包
    if(l.size() > 2)
    {
        // Antrn: 选中的所有组件排序
        addAndsortDiagramList(&dl, l);
//        for(auto i : dl)
//        {
//            qDebug() << "删减精简军军军军所军" << dynamic_cast<DiagramItem*>(i)->getIconName();
//        }
        // 暂时新搞个空的算法
        AlgorithmComp aac;
        aac.setDesc("这个是打包成的子模型，供测试使用。");
        // 基础信息map
        QMap<QString, QString> infomap;
        QMap<QString, QMap<QString, QString> >paramap;
        // 生成唯一id
        QString sid_new = QUuid::createUuid().toString();
        infomap.insert("ID", sid_new);
        infomap.insert("Name", new_name);
        infomap.insert("Path", QDir::currentPath() + "/radar/" + this->getEquip_id() + "/room/algoXml");
        // 获取日期
        QDateTime *dt = new QDateTime;
        QString dtime = dt->currentDateTime().toString();
        infomap.insert("Time", dtime);
        infomap.insert("callFunction", callFunc);
        infomap.insert("CN_Name", cnName);
        aac.setInfo(infomap);
        aac.setFileName(new_name);



        // Antrn: 上一个组件的输出端口，后一个组件的输入接口
        ItemPort *headInPort = nullptr, *tailOutPort = nullptr;
        // Antrn: 打包形成的组件的之前一个接口和后面一个端口
        ItemPort *lastOutPort = nullptr, *nextInPort = nullptr;
        QString lpname, bpname;
//        qDebug() << "获得所有ap的desc";
        // Antrn: 将列表中组件遍历一下将参数和端口信息填进来
        for(auto d : dl)
        {
//            qDebug() << "手机上看开始看看得到开口道";
            // 获取到该组件的AP类对象
            AlgorithmComp ap = scene->getScene_comps().take(d->itemSuuid);
            // Antrn: 获取所有的参数map
            QMap<QString, QMap<QString, QString> > tp = ap.getParam();
//            qDebug() << "反反复复付付付付付付" << tp.keys().size() << tp.keys().at(0);
            // Antrn: 参数名
            for(auto p : tp.keys())
            {
                QMap<QString, QString> tm;
                // Antrn: 参数名的属性名
                for(auto v : tp.value(p).keys())
                {
                    // Antrn: 找到是否是输入属性
                    if(v.compare("isoutput") == 0)
                    {
                        // Antrn: 如果isoutput属性值是0，说明是输入组件
                        if(dl.indexOf(d) == 0 && tp.value(p).value(v).compare("0") == 0)
                        {
                            tm.insert(v, tp.value(p).value(v));
                            // Antrn: WARNINIG FIXME 先考虑前一个组件只有一个输出端口的情况，打包的第一个组件只有一个输入端口
                            for(auto iii : dynamic_cast<DiagramItem*>(d)->getMports().keys())
                            {
//                                qDebug() << dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->portName() << "+++++++++++++++++++" << dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->getPortFlags();
                                if(!dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->isOutput() && dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->getPortFlags() == 0)
                                {
                                    headInPort = dynamic_cast<DiagramItem*>(d)->getMports().value(iii);
                                    lpname = dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->portName();

                                    qDebug() << dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->portName() << "+++++++++++++++++++";
                                    if(headInPort->getConnections().size() > 0)
                                        lastOutPort = headInPort->getConnections().at(0)->port1();
                                }
                            }
                        }
                        else if(dl.indexOf(d) == dl.size() - 1 && tp.value(p).value(v).compare("1") == 0)
                        {
//                            qDebug() << "99999999999999999999输出端口" << v << "; 值: " << tp.value(p).value(v);
                            tm.insert(v, tp.value(p).value(v));
                            for(auto iii : dynamic_cast<DiagramItem*>(d)->getMports().keys())
                            {
                                if(dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->isOutput() && dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->getPortFlags() == 0)
                                {
                                    tailOutPort = dynamic_cast<DiagramItem*>(d)->getMports().value(iii);
                                    bpname = dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->portName();
                                    qDebug() << dynamic_cast<DiagramItem*>(d)->getMports().value(iii)->portName() << "===========";
                                    if(tailOutPort->getConnections().size() > 0)
                                        nextInPort = tailOutPort->getConnections().at(0)->port2();
                                }
                            }
                        }
                    }
                    else
                    {
                        tm.insert(v, tp.value(p).value(v));
                    }
                }
                paramap.insert(p, tm);
            }
        }
        aac.setParam(paramap);

        ui->listWidget->algorithms.insert(sid_new, aac);
        qDebug() << "刚增加的id:" << sid_new;
        qDebug() << "组件列表中的算法algorithms： " << ui->listWidget->algorithms.keys() << "; 大小： " << ui->listWidget->algorithms.size();
        // Antrn: 保存到组件库
        Utils::writeAlgorithmComp2Xml(aac);
        // Antrn: 保存到组件空间
        Utils::writeAlgorithmComp2Xml(aac, "/radar/" + getEquip_id() + "/room");
        // 添加一个组件
        emit ui->listWidget->add_one_Comp(aac);
        // 刷新列表
        emit ui->listWidget->toRefreshCompList();
        qDebug() << "-------[重要draglist]---------" << aac.getInfo()["Name"];
        // 参数为空，往组件库添加组件icon
        //                Utils::generateIcon(new_name, "");
        Utils::alert(QApplication::desktop()->screen()->rect(), "添加组件入库成功!");
//        qDebug() << "添加新组件成功:" << new_name;
        QPointF p = dl.first()->pos();

        // 新建uuid
//        QString sid_new = QUuid::createUuid().toString();
        // Antrn: 开始新建组件替换掉原有的部分
        DiagramItem *newItem = new DiagramItem(sid_new, new_name, itemMenu, getEquip_id());
        newItem->setBrush(scene->getMyItemColor());
        // 20200713 新增
        scene->idList.append(sid_new);

        // 位置向右下方偏移20像素
        QPointF pos_new(p.x() + 20, p.y() + 20);
        qDebug() << "打包后新组件的位置:" << pos_new;
        newItem->setPos(pos_new);
        // 加到场景中
        scene->addItem(newItem);
        newItem->addPort(new_name, QUuid::createUuid().toString(), 0, ItemPort::NamePort);
        newItem->addPort(cnName, QUuid::createUuid().toString(), 0, ItemPort::TypePort);

        QString algoPath = QDir::currentPath() + "/algoXml/" + new_name + ".xml";
        scene->getPara2Port(algoPath, newItem);
        emit itemInserted(newItem);
        scene->modifyXmlItems(pos_new, newItem);
        // Antrn: 生成文件到room空间中
        scene->createFile2zoom(sid_new);
        connect(newItem, &DiagramItem::showItemsProperties, this, &MainWindow_Radar::receiveItemsid2showProperties);
        //点击组件显示队列信息
        connect(newItem, &DiagramItem::show_comp_lineinfo, this, &MainWindow_Radar::receiveIteminformation);

        for(auto portid : newItem->getMports().keys())
        {
            if(newItem->getMports().value(portid)->portName().compare(lpname) == 0)
            {
                headInPort = newItem->getMports().value(portid);
            }
            else if(newItem->getMports().value(portid)->portName().compare(bpname) == 0)
            {
                tailOutPort = newItem->getMports().value(portid);
            }
        }
        // Antrn: 还需要续上前后的组件的箭头连接，恢复原来的状态
        // Antrn: 找到线段的开始点端口p1处所属的Item
        DiagramItem * startItem_1 = nullptr;
        if(lastOutPort)
            startItem_1 = lastOutPort->block();
//        DiagramItem * endItem_1 = headInPort->block();
//        DiagramItem * startItem_2 = tailOutPort->block();
        DiagramItem * endItem_2 = nullptr;
        if(nextInPort)
            endItem_2 = nextInPort->block();

        if (startItem_1 && newItem &&
                startItem_1->type() == DiagramItem::Type &&
                newItem->type() == DiagramItem::Type &&
                startItem_1 != newItem)
        {
            // Antrn: 新建箭头
            Arrow *arrow = new Arrow(startItem_1, newItem);
            arrow->setColor(scene->getMyItemColor());
            // Antrn: 新建id
            QString sid = QUuid::createUuid().toString();
            arrow->itemId = sid;
            arrow->setPort1(lastOutPort);
            // Antrn:
            arrow->setPort2(headInPort);
            arrow->setPos1(lastOutPort->scenePos());
            arrow->setPos2(headInPort->scenePos());
            startItem_1->addArrow(arrow);
            newItem->addArrow(arrow);
            // Antrn: 一定要注意加入场景中的id列表中
            scene->idList.append(sid);

            qDebug() << "新箭头的ID: " << sid << "; 所有id:" << scene->idList;
            //处理箭头的删除信号
            connect(arrow, &Arrow::deletearrow_signal, scene, &RadarScene::deal_arrowdelte);

            scene->addItem(arrow);
            isSave = false;
            toggleSaveXml(1);
            qDebug() << "xml由于箭头添加而改变";
            emit send2AppOutput("[提示]xml由于箭头添加而改变", 1);
            scene->modifyXmlArrows(arrow, startItem_1, newItem);
            arrow->updatePath();
        }

        if (newItem && endItem_2 &&
                newItem->type() == DiagramItem::Type &&
                endItem_2->type() == DiagramItem::Type &&
                newItem != endItem_2)
        {
            // Antrn: 新建箭头
            Arrow *arrow = new Arrow(newItem, endItem_2);
            arrow->setColor(scene->getMyItemColor());
            // Antrn: 新建id
            QString sid = QUuid::createUuid().toString();
            arrow->itemId = sid;
            arrow->setPort1(tailOutPort);
            arrow->setPort2(nextInPort);
            arrow->setPos1(tailOutPort->scenePos());
            arrow->setPos2(nextInPort->scenePos());
            newItem->addArrow(arrow);
            endItem_2->addArrow(arrow);
            // Antrn: 一定要注意加入场景中的id列表中
            scene->idList.append(sid);

            qDebug() << "新箭头的ID: " << sid << "; 所有id:" << scene->idList;
            //处理箭头的删除信号
            connect(arrow, &Arrow::deletearrow_signal, scene, &RadarScene::deal_arrowdelte);

            scene->addItem(arrow);
            isSave = false;
            toggleSaveXml(1);
            qDebug() << "xml由于箭头添加而改变";
            emit send2AppOutput("[提示]xml由于箭头添加而改变", 1);
            scene->modifyXmlArrows(arrow, newItem, endItem_2);
            arrow->updatePath();
        }
        // Antrn: 删除已经选中的组件
        deleteItem();
        return QString("组建封装成功");
    }
    else
    {
        return QString("组件数量过少，没必要封装成大组件");
    }
}

/**
 * @brief MainWindow_Radar::itemInserted
 * @param di 场景中的算法组件
 * @author Antrn
 * @date 2020-07-04
 * @brief 组件添加事件处理，当场景中插入一个组件后进行的一系列操作
 */
void MainWindow_Radar::itemInserted(DiagramItem *di)
{
    // 场景中新增组件，保存按钮状态打开，待保存
    isSave = false;
    toggleSaveXml(1);
//    qDebug() << "xml由于组件添加而改变";
//    emit send2AppOutput("xml由于组件添加而改变",1);
    // 当有图形项插入到场景中的时候，应该将指针组改为移动指针
    pointerTypeGroup->button(int(RadarScene::MoveItem))->setChecked(true);
    scene->setMode(RadarScene::Mode(pointerTypeGroup->checkedId()));
    // 取消原按钮的选中状态
    // buttonGroup->button(int(item->diagramType()))->setChecked(false);

    // 将组件库的代码复制一份进来
    QString d_name = di->getIconName();
//    qDebug() << "将场景中" << d_name << "的代码从组件库提取出来";
    // 当添加的组件是输入输出显示组件时进行标记，防止重复加入
    itemMarked(d_name);
    if(d_name.compare("display"))
    {
        // 这里把代码的文件夹也改为名字+id
        QString code_dir_name = QDir::currentPath() + "/radar/" + di->getRadar_id() + "/room/codes/" + d_name + di->itemSuuid.mid(1, di->itemSuuid.size() - 2) + "/";
        // Antrn: 不存在则创建当前组件代码的文件夹
        Utils::openDirOrCreate(code_dir_name);
    }
    else
    {
        qDebug() << "该组件" << d_name << "为显示组件，不生成代码文件夹";
    }
    // Antrn: 同上，只不过变成是waveData的文件夹
    QString waveData_dir_name = QDir::currentPath() + "/radar/" + di->getRadar_id() + "/room/waveData/" + d_name + di->itemSuuid.mid(1, di->itemSuuid.size() - 2) + "/";
    Utils::openDirOrCreate(waveData_dir_name);

#if 0
    // Antrn: 将原来直接复制的方案改为直接基于模板新建到指定位置
    // 当前组件的cpp代码文件
    QString code_fname = code_dir_name + d_name.toLower() + ".c";

    if(Utils::qCopyFile(QDir::currentPath() + "/codes/" + di->getIconName().toLower() + ".c", code_fname))
    {
        qDebug() << QStringLiteral("c: 组件库->组件空间复制成功");
    }
    else
        qDebug() << QStringLiteral("c: 组件库->组件空间复制失败，查看是否是组价库中缺失");

    // 当前组件的h代码文件
    QString code_hname = code_dir_name + d_name.toLower() + ".h";
    // 将组件库的代码文件复制过来
    if(Utils::qCopyFile(QDir::currentPath() + "/codes/" + di->getIconName().toLower() + ".h", code_hname)) //将h文件复制到新的文件路径下
    {
        qDebug() << QStringLiteral("h: 组件库->组件空间复制成功");
    }
    else
        qDebug() << QStringLiteral("h: 组件库->组件空间复制失败，查看是否是组价库中缺失");

    // 当前组件的txt代码文件
    QString code_tname = code_dir_name + d_name.toLower() + ".txt";
    // 将组件库的init代码文件复制过来
//    QFileInfo ft(code_tname);
    //获取文件名
//    qDebug() << "Init文件名" << ft.fileName();
    if(Utils::qCopyFile(QDir::currentPath() + "/codes/" + di->getIconName().toLower() + ".txt", code_tname)) //将h文件复制到新的文件路径下
    {
        qDebug() << QStringLiteral(".txt: 组件库->组件空间复制成功");
    }
    else
        qDebug() << QStringLiteral(".txt: 组件库->组件空间复制失败，查看是否是组价库中缺失");
#endif
    // Antrn: 代码生成，暂时弃用
//    new CodeWindow(di);
}


/**
 * @brief MainWindow_Radar::textInserted 插入文字处理函数
 */
void MainWindow_Radar::textInserted(QGraphicsTextItem *)
{
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "xml由于文字添加而改变";
//    emit send2AppOutput("xml由于文字添加而改变",1);
    //取消选中状态
//    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(RadarScene::Mode(pointerTypeGroup->checkedId()));
}

/**
 * @brief MainWindow_Radar::deleteItemArrowById 删除doc中的箭头和图形项
 * @param id item组件的id
 */
void MainWindow_Radar::deleteItemArrowById(QString id)
{
    QDomNode itemNode = scene->getDoc()->elementsByTagName("Items").at(0);
    QDomNodeList nodeList = itemNode.childNodes();
    for (int i = 0; i < nodeList.size(); i++)
    {
        if(nodeList.at(i).isElement())
        {
            QDomElement elem = nodeList.at(i).toElement();
            if(elem.attribute("id") == id)
            {
                itemNode.removeChild(nodeList.at(i));
                qDebug() << "没删除之前的idList" << scene->idList;
                // 将id列表中的那个id删除
                scene->idList.removeOne(id);
                qDebug() << "找到图形项，已删除,id=" << id;
                qDebug() << "Item scene idList" << scene->idList;
                emit send2AppOutput(QStringLiteral("[提示]找到图形项，已删除,id=") + id, 1);
                QString id_split = id.mid(1, id.length() - 2);
                qDebug() << "id_split: " << id_split;
                // 删除room/algoXml/下的临时xml
                Utils::deleteXmlFileByName(QDir::currentPath() + "/radar/" + getEquip_id() + "/room/algoXml/", id_split);
                // 删除代码文件
                QString codeName = scene->getScene_comps().value(id).getFileName();
                qDebug() << "查看scene_comps" << scene->getScene_comps().value(id).getFileName();
                qDebug() << "代码文件夹:" << codeName;
                // 删除组件时候，如果是输入组件，就把它设置为false。输出显示组件同理
                if(codeName.compare("INPUT") == 0)
                {
                    scene->setIsExistInput(false);
                    isInputExist = false;
                }
                else if(codeName.compare("OUTPUT") == 0)
                {
                    scene->setIsExistOutput(false);
                    isOutputExist = false;
                }
                else if(codeName.compare("display") == 0)
                {
                    scene->setIsExistDisplay(false);
                    isDisplayExist = false;
                }

//                Utils::deleteCppHFileByName(QDir::currentPath()+"/radar/"+getEquip_id()+"/room/codes/"+codeName+"/", codeName);
                Utils::deleteCodeDirByName(QDir::currentPath() + "/radar/" + getEquip_id() + "/room/", codeName + id_split);
                qDebug() << "删除之前的algorithm的MAP： " << scene->getScene_comps().keys();
                scene->deleteScene_comps(id);
                qDebug() << "删除之后的algorithm的MAP： " << scene->getScene_comps().keys();
                break;
            }
        }
    }

    QDomNode arrowNode = scene->getDoc()->elementsByTagName("Arrs").at(0);
    /**
     * 注意这里面不能先定义变量QDomNodeList arrowList = arrowNode.childNodes()；，然后下面使用arrowList，
     * 因为当你删除一个标签节点之后，总长度size变了，每次都要重新获取size，而且找到一次之后index从0开始继续遍历
     **/
    for (int j = 0; j < arrowNode.childNodes().size(); j++)
    {
        if(arrowNode.childNodes().at(j).isElement())
        {
            QDomElement elem2 = arrowNode.childNodes().at(j).toElement();
            QString arrowId = elem2.attribute("id");
//            qDebug() << "start_item_id: " << elem2.attribute("start_item_id") << "end_item_id" << elem2.attribute("end_item_id");
//            emit send2AppOutput("start_item_id: " + elem2.attribute("start_item_id")+ "end_item_id" + elem2.attribute("end_item_id"),1);
            if(elem2.attribute("start_item_id") == id || elem2.attribute("end_item_id") == id)
            {
                arrowNode.removeChild(arrowNode.childNodes().at(j));
                // 将id列表中的那个id删除
                scene->idList.removeOne(arrowId);
                qDebug() << "找到箭头，已删除,id=" << arrowId;
                qDebug() << "Arrow scene idList" << scene->idList;
                emit send2AppOutput(QStringLiteral("[提示]找到箭头，已删除,id=") + arrowId, 1);
                // [注意] 这里千万注意不要写成了0，不然会少删除item，item和arrow不对应导致程序崩溃
                j = -1;
            }
        }
    }
    qDebug() << "deleteItemArrowById()" << "结束";
}

/**
 * @brief MainWindow_Radar::deleteArrowById 通过id删除箭头
 * @param id 箭头的id
 */
void MainWindow_Radar::deleteArrowById(QString id)
{
    QDomNode arrowNode = scene->getDoc()->elementsByTagName("Arrs").at(0);
    for (int j = 0; j < arrowNode.childNodes().size(); j++)
    {
        if(arrowNode.childNodes().at(j).isElement())
        {
            QDomElement elem2 = arrowNode.childNodes().at(j).toElement();
            QString arrowId = elem2.attribute("id");
            if(elem2.attribute("id") == id)
            {
                arrowNode.removeChild(arrowNode.childNodes().at(j));
                // 将id列表中的那个id删除
                scene->idList.removeOne(arrowId);
                qDebug() << "单独找到箭头将它删除,id=" << arrowId;
                qDebug() << "单独Arrow scene idList" << scene->idList;
                emit send2AppOutput(QStringLiteral("[提示]单独找到箭头将它删除,id=") + arrowId, 1);
                break;
                // 20200713
//                j = -1;
            }
        }
    }
}

/**
 * @brief 删除场景中按钮触发
 */
void MainWindow_Radar::deleteItem()
{
    if(scene->selectedItems().isEmpty())
    {
        qDebug() << "没有要删除的选择对象";
        emit send2AppOutput("[警告]： 没有要删除的选择对象！", 0);
//        QMessageBox::information(this, "警告", "请先选择组件!", QMessageBox::Ok);
        Utils::alert(qApp->desktop()->rect(), "请先选择组件");
    }
    else
    {
        // 由于删除了组件，所以要设置保存状态为：否
        isSave = false;
        toggleSaveXml(1);
        qDebug() << "xml由于删除控件而改变";
//        emit send2AppOutput("xml由于删除控件而改变",1);
        // 删除相连的箭头
        foreach (QGraphicsItem *item, scene->selectedItems())
        {
            if (item->type() == Arrow::Type)
            {

                Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);

                arrow->startItem()->removeArrow(arrow);
                arrow->endItem()->removeArrow(arrow);
                for(ItemPort * ip : arrow->startItem()->ports())
                {
                    if(ip == arrow->port1())
                    {
                        ip->removeConnection(arrow);
                    }
                    if(ip == arrow->port2())
                    {
                        ip->removeConnection(arrow);
                    }
                }
//                arrow->port1()->removeConnection(arrow);
//                arrow->port2()->removeConnection(arrow);

                // 删除doc中的此图形项对应的标签
                deleteArrowById(arrow->itemId);
                // 20200713
                scene->removeItem(item);
//                delete item;
//                scene->removeItem(arrow);
//                delete arrow;
            }
        }
        DiagramItem *item_ = nullptr;
        foreach (QGraphicsItem *item, scene->selectedItems())
        {
            if (item->type() == DiagramItem::Type)
            {
                item_ = qgraphicsitem_cast<DiagramItem *>(item);
                item_->removeArrows();

                // 删除doc中的此图形项对应的标签
                deleteItemArrowById(item_->itemSuuid);
            }
            scene->removeItem(item);
        }
    }
}

void MainWindow_Radar::deal_Arrowdelete(QString ID)
{
    deleteItem();
}
/**
 * @brief 场景中复制一份组件的动作触发
 */
void MainWindow_Radar::copyItem()
{
    //先进行判断，如果是输入输出组件则不能进行复制
    // 每次只能复制一个组件
    QGraphicsItem *item_ =  scene->selectedItems().first();
    DiagramItem *ditem =  qgraphicsitem_cast<DiagramItem *>(item_);
    //进行判断，如果是输入输出组件则不能进行复制
    if(ditem->getIconName().compare("INPUT") == 0 || ditem->getIconName().compare("OUTPUT") == 0)
    {
        Utils::alert(qApp->desktop()->rect(), "输入输出组件不可复制!");
        return;
    }

    // 从当前组件复制
    AlgorithmComp acp =  scene->getScene_comps().take(ditem->itemSuuid);
    //receiveAlgo4listWidget 插入组件的第一步
    scene->receiveAlgo4listWidget(acp);

    // 新建uuid
    QString sid = QUuid::createUuid().toString();
    qDebug() << "main1新生成的sid: " << sid;
//    newItem->itemSuuid = sid;

    // 复制一份，不能使用原来的那个指针
    DiagramItem *newItem = new DiagramItem(sid, ditem->iconName, itemMenu, getEquip_id());

    newItem->setBrush(scene->getMyItemColor());


    // 位置向右下方偏移20像素
    QPointF pos_(item_->pos().x() + 20, item_->pos().y() + 20);
    qDebug() << "复制后的位置:" << pos_;
    newItem->setPos(pos_);
    scene->addItem(newItem);
    newItem->addPort(ditem->iconName, QUuid::createUuid().toString(), 0, ItemPort::NamePort);
    newItem->addPort(ditem->CN_Name, QUuid::createUuid().toString(), 0, ItemPort::TypePort);


    QString algoPath = QDir::currentPath() + "/algoXml/" + ditem->iconName + ".xml";
    scene->getPara2Port(algoPath, newItem);

    emit itemInserted(newItem);
    scene->modifyXmlItems(pos_, newItem);
    // 生成文件
    scene->createFile2zoom(sid);
    connect(newItem, &DiagramItem::showItemsProperties, this, &MainWindow_Radar::receiveItemsid2showProperties);

    //点击组件显示队列信息
    connect(newItem, &DiagramItem::show_comp_lineinfo, this, &MainWindow_Radar::receiveIteminformation);
}

/**
 * @brief 将场景中（组件空间）的组件添加到组件库中
 */
void MainWindow_Radar::addItem2Lib()
{
    // 首先应该会取到此组件的名字、属性、等信息，然后新建xml文件，写入相关信息，再刷新组件列表，读取
    QGraphicsItem *i_ =  scene->selectedItems().first();
    DiagramItem *di =  qgraphicsitem_cast<DiagramItem *>(i_);
    // 复制一份组件算法
    AlgorithmComp ac =  scene->getScene_comps().take(di->itemSuuid);
    qDebug() << "算法组件复制对象的id:" << ac.getInfo()["ID"];
    // 新id
    QString sid = QUuid::createUuid().toString();
    qDebug() << "main2新生成的sid: " << sid;

    // 新的map，保存基础信息
    QMap<QString, QString> newm;
    newm.insert("ID", sid);
    // 组件库的位置
    newm.insert("Path", QDir::currentPath() + "/algoXml");
    // 当前时间
    QDateTime *dt = new QDateTime;
    QString dtime = dt->currentDateTime().toString();
    newm.insert("Time", dtime);

    // 文字对话框 输入字符串
    QString dlgTitle = "输入对话框";
    QString txtLabel = "请输入唯一组件名";
    QString defaultInput = ac.getInfo()["Name"]; // 默认命名
    QLineEdit::EchoMode echoMode = QLineEdit::Normal; //正常文字输入
    bool ok = false;
    QString text = QInputDialog::getText(this, dlgTitle, txtLabel, echoMode, defaultInput, &ok);
    if (ok && !text.isEmpty() && text.compare(defaultInput) != 0)
        newm.insert("Name", text);
    else if((ok && text.isEmpty()) || (ok && text.compare(defaultInput) == 0))
    {
        QMessageBox::warning(this, "警告", "加入组件库的组件名不能为空且不能重复！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return;
    }
    else
    {
        return;
    }

    ac.setInfo(newm);
    ac.setFileName(text);
    // 生成新的Icon
    Utils::generateIcon(text, getEquip_id());
    // 5.9.8
//    qDebug() << "打印一下要加入组件库的信息： " << ac.getInfo().toStdMap();
    ui->listWidget->algorithms.insert(sid, ac);

    qDebug() << "algorithms： " << ui->listWidget->algorithms.keys() << "; 大小： " << ui->listWidget->algorithms.size();
    Utils::writeAlgorithmComp2Xml(ac);
    emit ui->listWidget->add_one_Comp(ac);
    emit ui->listWidget->toRefreshCompList(); //刷新列表
    Utils::alert(QApplication::desktop()->screen()->rect(), "添加组件入库成功!");
}

// Antrn: TODO 这里的波形展示先不动，后期增强
/**
 * @author Antrn
 * @date 2020-06-25
 * @brief MainWindow_Radar::showThisWave 展示当前组件处理后的波形
 */
void MainWindow_Radar::showThisWave()
{
    //weishi:每个组件只能显示一个信号显示页面，打开后无法点击显示波形
    showWaveGraphicAction->setEnabled(false);
    // 当前选中的那个
    QGraphicsItem *i_ =  scene->selectedItems().first();
    DiagramItem *di =  qgraphicsitem_cast<DiagramItem *>(i_);
    // 算法组件名字
    QString algoName = di->iconName;
//    qDebug() << "算法组件名字:" << algoName << "当前装备的id：" << getEquip_id();
    // 找这个雷达的算法组件中的数据，显示出来
    // TODO 判断这个是雷达开发的项目，还是别的，电子对抗或者目标，因为公用了mainwindow_radar这个类
#if 0
    QString ph = QDir::currentPath() + "/radar/" + this->getEquip_id() + "/room/waveData/" + algoName + di->itemSuuid.mid(1, di->itemSuuid.size() - 2) + "/";
    Utils::openDirOrCreate(ph);
    QDir dir(ph);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();
    int file_count = list.count();
    if(file_count == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("还未生成数据文件，暂时不可查看。");
        msgBox.setWindowTitle("警告");
        msgBox.exec();
        qDebug() << "还未生成数据文件！";
    }
    else
    {
        // TODO 读取数据，使用波形图展示
        // 这个也可以，但是太不专业，难看，先注释掉
//        RealTimeCurve *rtc = new RealTimeCurve(ph+"data.txt",nullptr);
//        rtc->show();

        RunCompConf *run = new RunCompConf(ph + "data.txt", nullptr);
        run->exec();
    }
#endif
    //weishi:当前组件为显示组件并且没有打开过其他信号显示窗口时，才能打开信号显示窗口
    if(algoName.compare("display") == 0 && isSignalShowOpen == false)
    {
//        //weishi:打开信号显示页面之前先自动调用cmd开始接收数据
//        QProcess *p = new QProcess(this);
//        QString s1 = "cmd.exe";
//        QStringList s2;
//        s2.append("//RspMain");
//        p->execute(s1, s2);
        RealTimeSignal *signal_widget = new RealTimeSignal();
        signal_widget->show();
        connect(signal_widget, &RealTimeSignal::isWindowClosed, this, &MainWindow_Radar::deal_isSignalShowOpen);
        isSignalShowOpen =true;
    }
    else if (algoName.compare("display") != 0)
    {
        qDebug() << "暂不支持查看单个其他组件（除了显示组件）的数据波形";
    }
    else if(isSignalShowOpen == true)
    {
        qDebug() << "信号显示页面已经打开，不能重复打开";
    }
}


/**
 * @brief 通过不同的按钮变换不同的背景
 * @param button
 */
void MainWindow_Radar::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons)
    {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("海洋方格"))
        scene->setBackgroundBrush(QPixmap(":/img/ocean.png"));
    else if (text == tr("草地方格"))
        scene->setBackgroundBrush(QPixmap(":/img/grass.png"));
    else if (text == tr("灰色方格"))
        scene->setBackgroundBrush(QPixmap(":/img/background3.png"));
    else if (text == tr("天空背景"))
        scene->setBackgroundBrush(QPixmap(":/img/sky.png"));
    else if (text == tr("沙漠背景"))
        scene->setBackgroundBrush(QPixmap(":/img/desert.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/img/background4.png"));

    scene->update();
    ui->graphicsView->update();
}

#if 0
void MainWindow_Radar::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons)
    {
        if (buttonGroup->button(id) != button)
            button->setChecked(false);
    }
    //判断是否是文本框
    if (id == InsertTextButton)
    {
        scene->setMode(RadarScene::InsertText);
    }
    else
    {
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(RadarScene::InsertItem);
    }
}
#endif

/**
 * @brief 弹出属性窗口，只有输入输出组件会弹出，这里用了字符串匹配，当用户更改了INPUT和OUTPUT之后，会出问题
 */
void MainWindow_Radar::showItemProperties()
{
    // 当框选中多个组件，右键属性，也只会选中第一个，所以没问题
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem *item_ = dynamic_cast<DiagramItem*>(item);
            // FIXME 字符串匹配组件名
            if(item_->iconName.compare("INPUT") == 0 || item_->iconName.compare("OUTPUT") == 0)
            {
                qDebug() << "Show Component Property" << item_->iconName;
//                emit send2AppOutput("显示组件属性" + item_->iconName,1);
                // DiagramItem对象增加comp，输入输出窗口
                if(item_->comp->getPath().isNull())
                {
                    item_->comp->setPath(scene->getScene_comps()[item_->itemSuuid].getParam()["file"]["value"]);
                }
                qDebug() << item_->comp->getPath();
                item_->comp->setUid(item_->itemSuuid);
                item_->comp->setEquipId(item_->getRadar_id());
                item_->comp->setScene(scene);
                item_->comp->exec();
            }
            else
            {
                qDebug() << "其他组件不能配置输入输出";
                QMessageBox::information(nullptr, "提示", "该组件不支持此选项", QMessageBox::Ok);
            }
            break;
        }
    }
}
//shao::单个组件参数配置
void MainWindow_Radar::comp_para_config()
{

    QGraphicsItem *item = scene->selectedItems().first();
    DiagramItem *item_ = nullptr;
    item_ = qgraphicsitem_cast<DiagramItem *>(item);
    AlgorithmComp ac = this->scene->getScene_comps().value(item_->itemSuuid);
    this->main_process->getComponent_process()->parameter_configuration(ac, item_);
    connect(this->main_process->getComponent_process(), &Component_process::edit_succeed, this, [&, item_](AlgorithmComp * ac)
    {
        //取出来 更新 放回去
        QMap<QString, AlgorithmComp> map = this->scene->getScene_comps();
        map[item_->itemSuuid] = *ac;
        this->scene->setScene_comps(map);
    });
}
//选择组件使用的CPU
void MainWindow_Radar::chooseCPU()
{
    //qDebug() <<"选择CPU";
    cpu_dialog = new comp_cpu();

    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem *item_ = dynamic_cast<DiagramItem*>(item);
            cpu_dialog->setname(item_->iconName);
            cpu_dialog->setID(item_->itemSuuid);
        }
    }
    cpu_dialog->show();
    connect(cpu_dialog, &comp_cpu::IPaddress_choosed, this, &MainWindow_Radar::deal_IPaddress);
}

void MainWindow_Radar::deal_IPaddress(QString ip)
{
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type)
        {
            //找到场景中的被选中组件并设置它的Ip地址
            DiagramItem *item_ = dynamic_cast<DiagramItem*>(item);
            item_->setIn_whichIP(ip);
            qDebug() << "测试：当前组件选中的ip为" << ip;
            qDebug() << "测试：组件自身的ip是" << item_->getIn_whichIP();
            // Antrn: 更新组件对应的代码中的component_ip的值
            QTextCodec *codec = QTextCodec::codecForName("utf-8");
            QString allCodes;
            QStringList listCodes;
            QFile cppFile(QDir::currentPath() + "/radar/" + getEquip_id() + "/room/codes/" + item_->getIconName() + item_->itemSuuid.mid(1, item_->itemSuuid.length() - 2) + "/" + item_->getIconName() + ".cpp");
            if(cppFile.exists() && cppFile.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                QTextStream in(&cppFile);
                in.setCodec(codec);
                while(!in.atEnd())
                {
                    allCodes += in.readLine() + "\n";
                }
                listCodes = allCodes.split("\n");
                int index = listCodes.indexOf(QRegExp("*component_ip*", Qt::CaseSensitive, QRegExp::Wildcard)); //searching for string with *APPLICATION* wildcard
                listCodes[index] = "\tcomponent_ip = \"" + item_->getIn_whichIP() + "\";"; //replacing string on QString* app
                allCodes = "";
                allCodes = listCodes.join("\n"); // from QStringList to QString
                cppFile.seek(0);
                QTextStream out(&cppFile);
                out.setCodec(codec);
                out << allCodes; //writing to the same file
                cppFile.close();
            }
            else
            {
                qDebug() << "cannot read the file Or file not exists！";
            }
        }
    }
}

//选择组件使用的线程
void MainWindow_Radar::chooseThread()
{
    //qDebug() <<"选择线程";
    thread_dialog = new comp_thread();

    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type)
        {
            DiagramItem *item_ = dynamic_cast<DiagramItem*>(item);
            thread_dialog->setname(item_->iconName);
            thread_dialog->setID(item_->itemSuuid);
        }
    }

    thread_dialog->show();
    connect(thread_dialog, &comp_thread::threadnum_inserted, this, &MainWindow_Radar::deal_threadnum);
}

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-09-28 T 14:53:26
 * @copyright (c)
 * @brief MainWindow_Radar::addAndsortDiagramList 此时列表是无序的空列表，需要传入指针，然后排序
 * @param diagramlist 组件列表
 * @param list 选中的/场景中所有的组件列表
 */
void MainWindow_Radar::addAndsortDiagramList(QList<DiagramItem *> *diagramlist, QList<QGraphicsItem*> list)
{
    // Antrn: NOTE 这里循环次数写死了，因为由于用户在场景中连线分支的数量不同，循环次数也得增加，但是这里暂时共30次循环，因为自己测试的图中分支数不多
    for(int j = 0; j < 30; j++)
    {
        for (auto i : list)
        {
            Arrow *arrow_temp;
            // 每个箭头只遍历一遍
            if((arrow_temp = dynamic_cast<Arrow*>(i)))
            {
                if(arrow_temp->startItem()->index == -1)
                {
                    arrow_temp->startItem()->index = 1;
                    if(arrow_temp->endItem()->index < 2)
                    {
                        arrow_temp->endItem()->index = 2;
                    }
                }
                else
                {
                    if(arrow_temp->startItem()->index + 1 > arrow_temp->endItem()->index)
                    {
                        arrow_temp->endItem()->index = arrow_temp->startItem()->index + 1;
                    }

                }
//                qDebug() << "是箭头" << arrow_temp->itemId << arrow_temp->startItem()->index << arrow_temp->endItem()->index;
            }
        }
    }

    // Antrn: 再来遍历一次计算即可保证顺序
    for (auto i : list)
    {
        DiagramItem *temp;
        // TODO 要按照箭头的顺序来插入列表
        Arrow *arrow_temp;
        // 每个箭头只遍历一遍
        if((arrow_temp = dynamic_cast<Arrow*>(i)))
        {
            if(arrow_temp->startItem()->index == -1)
            {
                arrow_temp->startItem()->index = 1;
                if(arrow_temp->endItem()->index < 2)
                {
                    arrow_temp->endItem()->index = 2;
                }
            }
            else
            {
                if(arrow_temp->startItem()->index + 1 > arrow_temp->endItem()->index)
                {
                    arrow_temp->endItem()->index = arrow_temp->startItem()->index + 1;
                }
            }
//            qDebug() << "箭头两端（起始和终点权值积累：）" << arrow_temp->startItem()->getIconName() << arrow_temp->startItem()->index << arrow_temp->endItem()->getIconName() << arrow_temp->endItem()->index;
        }
        else if((temp = dynamic_cast<DiagramItem*>(i)))
        {
            diagramlist->append(temp);
        }
    }
    // Antrn: 自定义排序算法，按照每个DiagramItem的index索引的大小将list中的对象进行从小到大排序。但这样的话，如果有并行分支，两个组件的index一样，再插入list时也会有前后顺序
    qSort(diagramlist->begin(), diagramlist->end(), [](const DiagramItem * d1, const DiagramItem * d2)
    {
        return d1->index < d2->index;
    });

    // Antrn: ↑到这里已经按照index从小到大对算法组件排好序
}

//设置组件的线程数属性槽函数
void MainWindow_Radar::deal_threadnum(int num)
{
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == DiagramItem::Type)
        {
            //找到场景中的被选中组件并设置它的线程数
            DiagramItem *item_ = dynamic_cast<DiagramItem*>(item);
            item_->setNum_thread(num);
            qDebug() << "测试：当前组件的线程数为" << num;
            qDebug() << "测试：组件自身的线程数属性是" << item_->getNum_thread();
        }
    }
}
// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-28
 * @brief MainWindow_Radar::closeEvent // 先判断是否退出钱保存？是，则保存退出；否，则直接退出。
 * @param event
 */
void MainWindow_Radar::closeEvent(QCloseEvent *event)
{
    SysMessage::close_sys_message();
    qDebug() << "关闭！！！";

    //还未保存
    if(isSave == false)
    {
//        this->showNormal();
        this->raise();
        int ret1 = QMessageBox::question(this, tr("确认"), tr("确定退出前保存场景?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(ret1 == QMessageBox::Yes)
        {
            save2XmlFile();
            isSave = true;
            toggleSaveXml(0);
            ui->actionRunRadar->setEnabled(true);
            qDebug() << "保存退出";
            emit send2AppOutput("[提示]保存退出", 1);
            //提醒父类更新列表
            emit iClose(this);
            event->accept();
//            MainWindowNew::main_radar_list.removeOne(this);

        }
        else if(ret1 == QMessageBox::No)
        {
            qDebug() << "do not save";
            //提醒父类更新列表
            emit iClose(this);
            // 直接退出
            event->accept();
//            MainWindowNew::main_radar_list.removeOne(this);
        }
        else
        {
            // 拒绝关闭
            qDebug() << "拒绝关闭!!!";
            event->ignore();
        }
    }
    else
    {
        //提醒父类更新列表
        emit iClose(this);
        event->accept();
//        MainWindowNew::main_radar_list.removeOne(this);

    }
}

//组件层叠关系
void MainWindow_Radar::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "xml由于组件上移一层而改变";
}

/**
 * @brief MainWindow_Radar::sendToBack 放置到面板后面
 */
void MainWindow_Radar::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    // 注意 这个排序应该是从浅入深
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems)
    {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    //只把selectedItem设置为最高的z-index
    selectedItem->setZValue(zValue);
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "xml由于组件下移一层而改变";
}

//create相关
// This is available in all editors.
/**
* @projectName   prototype_v0902
* @brief         修改： 组件名修改
* @author        Antrn
* @date          2019-09-03
*/
void MainWindow_Radar::createCompBox()
{
    loadAllComps();

#if 0
    init5Comp();
    //-----------旧版使用buttonGroup-----------------------
    buttonGroup = new QButtonGroup(ui->dockWidgetContents);
    buttonGroup->setExclusive(true);
    //自定义的slot函数：与graphics交互的关键
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    // TODO 支持点击修改组件名
    layout->addWidget(createCellWidget(tr("脉冲压缩"), DiagramItem::Comp1), 0, 0); // Component_1 0,0
    layout->addWidget(createCellWidget(tr("恒虚警率"), DiagramItem::Comp2), 0, 1); // Component_2 0,1
    layout->addWidget(createCellWidget(tr("输入"), DiagramItem::Comp3), 1, 1); // Component_3 1,1
    layout->addWidget(createCellWidget(tr("动目标检测"), DiagramItem::Comp4), 1, 0); // Component_4 1,0
    layout->addWidget(createCellWidget(tr("输出"), DiagramItem::Comp5), 2, 0); // Component_5 2,0
    // 文字按钮，在场景添加文字，暂时保留
    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/img/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    //1,0表示1行0列 TODO 文字
    ClickableLabel *text = new ClickableLabel(tr("Text"));
    // 能被选中
    connect(text, &ClickableLabel::lostFocus, text, &ClickableLabel::labelLostFocus);
    text->setTextInteractionFlags(Qt::TextSelectableByMouse);
    textLayout->addWidget(text, 1, 0, Qt::AlignCenter);

    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 2, 1); // 2,1

    //设置行和列的比例
    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

#endif
    //______________________背景颜色---------------------------

    backgroundButtonGroup = new QButtonGroup(ui->dockWidgetContents);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("海洋方格"),
                                ":/img/ocean.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("草地方格"),
                                ":/img/grass.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("灰色方格"),
                                ":/img/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("无背景"),
                                ":/img/background4.png"), 1, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("天空背景"),
                                ":/img/sky.png"), 2, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("沙漠背景"),
                                ":/img/desert.png"), 2, 1);
    backgroundLayout->setRowStretch(3, 10);
    backgroundLayout->setColumnStretch(3, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);

    //---------------------------------------------------

    //========布局搜索框和listWidget=============
    QVBoxLayout *searchlayout = new QVBoxLayout;
    searchlayout->setContentsMargins(1, 1, 1, 1);
    ui->dockWidgetContents->setStyleSheet("#dockWidgetContents{border:1px solid gray;}");
    QHBoxLayout *pSearchLayout = new QHBoxLayout;
    m_pSearchLineEdit = new QLineEdit;

    word_list << "Java" << "C++" << "Python" << "脉冲压缩" << "恒虚警率" << "输入" << "输出" << "动目标检测" << "FDPC" << "MTD" << "CFAR";
    QCompleter *completer = new QCompleter;
    string_list_model = new QStringListModel(word_list, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(string_list_model);
    m_pSearchLineEdit->setCompleter(completer);
    // 自动补全
    connect(m_pSearchLineEdit, SIGNAL(editingFinished()), this, SLOT(editComplete()));

    m_pSearchLineEdit->setStyleSheet("height:24px;");
    m_pSearchLineEdit->setPlaceholderText(tr("输入组件名称"));
    QPushButton *pSearchButton = new QPushButton;
    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setToolTip(QStringLiteral("搜索"));
    pSearchButton->setFixedSize(20, 20);
    pSearchButton->setStyleSheet("QPushButton{border-image:url(:/img/search.png); background:transparent;} \
                                         QPushButton:hover{border-image:url(:/img/search_hover.png)} \
                                         QPushButton:pressed{border-image:url(:/img/search_press.png)}");
    //防止文本框输入内容位于按钮之下
    QMargins margins = m_pSearchLineEdit->textMargins();
    m_pSearchLineEdit->setTextMargins(margins.left(), margins.top(), pSearchButton->width(), margins.bottom());
    m_pSearchLineEdit->setPlaceholderText(QStringLiteral("请输入搜索内容"));
    pSearchLayout->addStretch();
    pSearchLayout->addWidget(pSearchButton);
    pSearchLayout->setSpacing(0);
    pSearchLayout->setContentsMargins(0, 0, 0, 0);
    m_pSearchLineEdit->setLayout(pSearchLayout);

    connect(pSearchButton, SIGNAL(clicked(bool)), this, SLOT(search()));

    searchlayout->addWidget(m_pSearchLineEdit);
    searchlayout->addWidget(ui->listWidget);
    QWidget *bigW = new QWidget;
    searchlayout->setStretch(0, 0);
    searchlayout->setStretch(1, 12);
    bigW->setLayout(searchlayout);
    //========================

    QToolBox *toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored));
//    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->setMinimumWidth(backgroundWidget->sizeHint().width());
    toolBox->addItem(bigW, tr("组件列表"));
//    toolBox->addItem(itemWidget, tr("基础雷达组件"));
    toolBox->addItem(backgroundWidget, tr("背景设置"));
    toolBox->setItemIcon(0, QIcon(":/img/compXmlDock.png"));
    toolBox->setItemIcon(1, QIcon(":/img/bgList.png"));
    // TODO 如何只在组件打开的时候才显示属性框？或者可以将这三个分成三个小窗口
    ui->dockCompList->setWidget(toolBox);
}

/**
 * @brief MainWindow_Radar::createActions 新建动作对象
 */
void MainWindow_Radar::createActions()
{
    toFrontAction = new QAction(QIcon(":/img/bringtofront.png"), tr("前移一层"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("前移一层"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));

    sendBackAction = new QAction(QIcon(":/img/sendtoback.png"), tr("后移一层"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("后移一层"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    // 删除场景中的组件动作
    deleteAction = new QAction(QIcon(":/img/delete.png"), tr("删除"), this);
    deleteAction->setShortcut(tr("delete"));
    deleteAction->setStatusTip(tr("将此组件从场景中删除"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));

    copyAction = new QAction(QIcon(":/img/copy.png"), tr("复制"), this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("复制一份此组件"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copyItem()));

    add2CompLibraryAction = new QAction(QIcon(":/img/add2Lib.png"), tr("加入组件库"), this);
//    add2CompLibraryAction->setShortcut(tr("Ctrl+A"));
    add2CompLibraryAction->setStatusTip(tr("将此组件添加到组件库"));
    connect(add2CompLibraryAction, SIGNAL(triggered()), this, SLOT(addItem2Lib()));

    showWaveGraphicAction = new QAction(QIcon(":/img/showWave.png"), tr("显示波形图"), this);
    showWaveGraphicAction->setStatusTip(tr("显示当前组件处理后的波形图"));
    connect(showWaveGraphicAction, SIGNAL(triggered()), this, SLOT(showThisWave()));

    codeEditAction = new QAction(QIcon(":/img/code.png"), tr("编辑算法代码"), this);
//    codeEditAction->setShortcut(tr("Ctrl+E"));
    codeEditAction->setStatusTip(tr("编辑此组件的代码"));
    connect(codeEditAction, SIGNAL(triggered()), this, SLOT(codeEditSlot()));

    // 查看属性动作
    propertyAction = new QAction(QIcon(":/img/property.png"), tr("输入输出"), this);
//    propertyAction->setShortcut(tr("Ctrl+R"));
    propertyAction->setStatusTip(tr("配置组件的输入输出"));
    connect(propertyAction, &QAction::triggered, this, &MainWindow_Radar::showItemProperties);


    exitAction = new QAction(QIcon(":/img/exit.png"), tr("退出"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("离开雷达编辑窗口"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(false);
    QPixmap pixmap(":/img/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon(":/img/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(false);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/img/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(false);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    //shao:组件参数配置
    para_config_action = new QAction(QIcon(":/img/curve.png"), tr("参数配置"), this);
    para_config_action->setStatusTip(tr("当前组件的参数配置"));
    connect(para_config_action, SIGNAL(triggered()), this, SLOT(comp_para_config()));

    //选择CPU
    chooseCPU_Action = new QAction(QIcon(":/img/curve.png"), tr("选择CPU"), this);
    chooseCPU_Action->setStatusTip(tr("选择当前组件使用的CPU"));
    connect(chooseCPU_Action, SIGNAL(triggered()), this, SLOT(chooseCPU()));

    //选择线程
    chooseThread_Action = new QAction(QIcon(":/img/data.png"), tr("选择线程"), this);
    chooseThread_Action->setStatusTip(tr("选择当前组件使用的线程"));
    connect(chooseThread_Action, SIGNAL(triggered()), this, SLOT(chooseThread()));

    //初始化为不可单击
    toggleSaveXml(0);
    ui->actionRunRadar->setEnabled(false);
}

// This is an auto-generated comment.
/**
* @brief      简介 添加选项卡选项
*/
void MainWindow_Radar::createMenus()
{
//    文件选项卡写到了ui文件里，就去掉
//  fileMenu = menuBar()->addMenu(tr("&文件"));
    ui->menu_files->addAction(exitAction);

//  itemMenu = menuBar()->addMenu(tr("&组件"));
    itemMenu = ui->menu_component;
    itemMenu->addAction(deleteAction);
    itemMenu->addAction(propertyAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);
    itemMenu->addSeparator();
    itemMenu->addAction(copyAction);
    itemMenu->addAction(add2CompLibraryAction);
    itemMenu->addAction(showWaveGraphicAction);
    itemMenu->addAction(codeEditAction);
    itemMenu->addSeparator();
    itemMenu->addAction(para_config_action);
    itemMenu->addAction(chooseCPU_Action);
    itemMenu->addAction(chooseThread_Action);
    // 一开始先设置不能用
    itemMenu->setEnabled(false);
    // 比例条
    progressBar = new QProgressBar;
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(0);

    // 运行时间
//    label_time=new QLabel(tr("Running： "),this);
    // 把时间标签加在状态栏中
//    ui->statusbar->addPermanentWidget(label_time);
//    ui->statusbar->addPermanentWidget(progressBar);
}

/**
 * @brief 添加工具栏按钮 拖动报错，考虑没用的话去掉改成一个工具栏
 */
void MainWindow_Radar::createToolbars()
{
    editToolBar = addToolBar(tr("编辑"));

    // 打包
    save2LibButton = new QToolButton;
    save2LibButton->setToolTip("将场景中已选中的组件封装成大组件，并入库");
    save2LibButton->setIcon(QIcon(":/img/pkg.png"));
    // 初始化禁用按钮
    save2LibButton->setEnabled(false);
    // 保存到组件库
    connect(save2LibButton, SIGNAL(clicked()),
            this, SLOT(libButtonTriggered()));
    // 加入到第一个工具栏中
    editToolBar->addWidget(save2LibButton);

    // 不显示
//    editToolBar->setHidden(true);
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);

#if 0
    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    //bottom: 2 top: 64
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/img/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/img/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));
#endif
    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineColorToolButton->setToolTip("选中改变连线的颜色");
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/img/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));


#if 0
    textToolBar = addToolBar(tr("Font"));
    // 默认不显示
    textToolBar->setHidden(true);
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);
#endif

    colorToolBar = addToolBar(tr("颜色"));
    colorToolBar->setToolTip("选中改变连线的颜色");
    // 默认不显示
    colorToolBar->setHidden(false);
//    colorToolBar->addWidget(fontColorToolButton);
//    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    //默认是指针移动
    pointerButton->setIcon(QIcon(":/img/pointer.png"));
    pointerButton->setToolTip("切换为选择模式");

    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/img/linepointer.png"));
    linePointerButton->setToolTip("切换为选择模式");

    pointerTypeGroup = new QButtonGroup(this);
    //后面参数是id唯一标识
    pointerTypeGroup->addButton(pointerButton, int(RadarScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(RadarScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    sceneScaleCombo->setToolTip("改变场景的缩放比例");
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);//100%
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("鼠标"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addSeparator();
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addSeparator();
    pointerToolbar->addWidget(sceneScaleCombo);

}

QWidget *MainWindow_Radar::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

#if 0
QWidget *MainWindow_Radar::createCellWidget(const QString &text, DiagramItem::DiagramType type)
{
    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
#endif

QMenu *MainWindow_Radar::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i)
    {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon MainWindow_Radar::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}

QIcon MainWindow_Radar::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}

//视图快照
/**
* @projectName   prototype_v0719
* @brief         保存场景或者视图快照
* @author        Antrn
* @date          2019-08-12
*/
// NOTE 这里也是将保存的图片直接保存到默认文件夹，也就是当前桌面文件夹下，后期需要改动
void MainWindow_Radar::saveSnapshot(int flag)
{
    // 当前路径
    QString currDir = QDir::currentPath();
    // 桌面路径
    QString deskTop =  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    switch (flag)
    {
    case 1:
    {
//            QSize mysize(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
//            QImage image(mysize, QImage::Format_RGB32);
//            QPainter painter(&image);
//            painter.setRenderHint(QPainter::Antialiasing);
//            scene->render(&painter);//也可以使用视图保存，只保存视图窗口的快照
//            painter.end();

        QString dir_str = deskTop + "/snapshots/";
////            QString dir_str = currDir+"/snapshots/";
//            // 检查目录是否存在，若不存在则新建
//            QDir dir;
//            if (!dir.exists(dir_str))
//            {
//                dir.mkpath(dir_str);
//            }
//            image.save(dir_str+"scene.png");

        Utils::saveImage(flag, scene, nullptr, dir_str, "scene.png");
        break;
    }
    case 2:
    {
//            QSize mysize(static_cast<int>(ui->graphicsView->width()), static_cast<int>(ui->graphicsView->height()));
//            QImage image(mysize, QImage::Format_RGB32);
//            QPainter painter(&image);
//            painter.setRenderHint(QPainter::Antialiasing);
//            ui->graphicsView->render(&painter);//也可以使用视图保存，只保存视图窗口的快照
//            painter.end();

        QString dir_str = deskTop + "/snapshots/";
//            QString dir_str = currDir+"/snapshots/";
        // 检查目录是否存在，若不存在则新建
//            QDir dir;
//            if (!dir.exists(dir_str))
//            {
//                dir.mkpath(dir_str);
//            }
//            image.save(dir_str+"view.png");
        Utils::saveImage(flag, nullptr, ui->graphicsView, dir_str, "view.png");
        break;
    }
    }
}

bool MainWindow_Radar::getIsDisplayExist() const
{
    return isDisplayExist;
}

void MainWindow_Radar::setIsDisplayExist(bool value)
{
    isDisplayExist = value;
}

//handleFontChange相关
void MainWindow_Radar::currentFontChanged(const QFont &)
{
    handleFontChange();
}

void MainWindow_Radar::fontSizeChanged(const QString &)
{
    handleFontChange();
}

void MainWindow_Radar::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}


//ColorChanged相关
void MainWindow_Radar::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/img/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void MainWindow_Radar::itemColorChanged()
{
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "组件颜色改变";
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/img/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void MainWindow_Radar::lineColorChanged()
{
    isSave = false;
    toggleSaveXml(1);
    qDebug() << "线段颜色改变";
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/img/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void MainWindow_Radar::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void MainWindow_Radar::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void MainWindow_Radar::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

//将当前场景中选中的组件打包加入组件库中
// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-09-29 T 17:45:04
 * @copyright (c)
 * @brief MainWindow_Radar::libButtonTriggered
 */
void MainWindow_Radar::libButtonTriggered()
{
    qDebug() << "将当前场景中框选的组件打包加入组件库中，形成一个较大型子组件";
    // Antrn: 用户是否点击OK
//    bool ok;
    // Antrn: 用户赋予新组件的名字
    QString new_name, cn_name;
    // Antrn: 消息询问框
    QMessageBox msgBox(this);
    msgBox.setText("检测到您已经框选多个组件。");
    msgBox.setInformativeText("是否打算将所框选的组件封装后加入组件库？");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle("询问");
    // Antrn: 结果
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:
    {
//        new_name = QInputDialog::getText(this, tr("组件封装"), tr("请给将所框选的组件设置一个唯一组件名称"), QLineEdit::Normal, nullptr, &ok);
        qDebug() << "打开窗口";
        // Antrn: 下面的3和list中数量对应
        InfoInputDialog dialog(3, ui->listWidget->nameList, this, nullptr);
        QStringList list;
        list << "英文缩写: " << "中文名: " << "业务库函数名: ";
        dialog.SetLabelTexts(list);
        dialog.SetLabelsWidth(300);
        dialog.exec();
#if 0
        // 点击 ok
        if (ok && !new_name.isEmpty())
        {
            // 检查组件库列表中是否已有该名字
            if(ui->listWidget->nameList.contains(new_name))
            {
                qDebug() << "与现有的文件名: " << new_name << "重复，添加失败！";
                emit send2AppOutput("[警告]： 已有重复名称存在，请重新新建组件！", 0);
            }
            else
            {
                // 生成在本雷达下的icon图像
//                ui->listWidget->send_icon_name(new_name);

                // 封装成子组件，加入组件库
                QString resultTip = makeSelectedItems2OneComp(new_name, cn_name);
                Utils::alert(qApp->desktop()->rect(), resultTip);
            }
        }
        else
        {
            qDebug() << "点击取消/×/空";
        }
#endif
        break;
    }
    case QMessageBox::Cancel:
        // Antrn: 取消，什么也不做
        break;
    default:
        break;
    }
}

/**
 * @brief 选择并打开.rad雷达工程文件，导入场景中
 */
void MainWindow_Radar::on_actionOpenXml_triggered()
{
    QString dirpath = QDir::currentPath() + "/radar/";
    Utils::openDirOrCreate(dirpath);
    // 打开rad文件读取
    const QString fileName = QFileDialog::getOpenFileName(this, tr("选择打开.rad工程文件！"), QString(dirpath), tr("rad files (*.rad)"));
    // 查看场景中是否为空，若是才能导入，否则提醒清空场景
    // 也可以直接scene.clear()
    if(scene->items().isEmpty())
    {
        // 直接把选择的这个雷达工程中的algoXml文件夹复制过来
        QString fromdirname = fileName.mid(0, fileName.lastIndexOf("/") + 1) + "room/algoXml/";
        QString todirname = QDir::currentPath() + "/radar/" + getEquip_id() + "/room/algoXml/";
        qDebug() << "fromdirname" << fromdirname << "todirname" << todirname;
        bool tof = Utils::qCopyDirectory(QDir(fromdirname), QDir(todirname), true);
        qDebug() << "是否复制成功?" << tof;

        readXmlConf(fileName);
    }
    else
    {
        // TODO 还有一种方法，默认导入按钮不可用,当场景中组件为空时，则导入按钮可用
        emit send2AppOutput("[警告]： 现在的场景中存在组件未清空！", 0);
    }
}

// This is an auto-generated comment.
/**
* @projectName   prototype_v0906
* @brief         简介 读取场景的xml文件： .rad
* @author        Antrn
* @date          2019-10-07
*/
void MainWindow_Radar::readXmlConf(QString xmlname)
{
    QDomDocument doc;
    if(!xmlname.isEmpty())
    {
        QFile file(xmlname);
        if(!file.open(QIODevice::ReadOnly)) return;
        if(!doc.setContent(&file))
        {
            file.close();
            qDebug() << "打开失败";
            return;
        }
        file.close();
        //根元素component
        QDomElement docElem = doc.documentElement();
        int wid, hei;
        wid = docElem.attribute("width").toInt();
        hei = docElem.attribute("height").toInt();
        scene->setSceneRect(QRectF(0, 0, wid, hei));
        // 第一个孩子是Arrs或者Items，不能这样获取
//        QDomNode n = docElem.firstChild();
        QDomNode itemNode = doc.elementsByTagName("Items").at(0);
        QString id;
        // 子孩子就是标签名为comp_1...
        QDomNode m = itemNode.firstChild();
        // 临时节点
        QDomNode temp;
        while(!m.isNull())
        {
            temp.clear();
            std::string tagName = m.nodeName().toStdString();
            if(m.isElement())
            {
                // 每个元素item
                QDomElement e = m.toElement();
                id = e.attribute("id");
                qreal posx = e.attribute("pos_x").toInt();
                qreal poxy = e.attribute("pos_y").toInt();
                QString s = e.elementsByTagName("color").at(0).toElement().text();
                QString cf = e.elementsByTagName("call").at(0).toElement().text();
                QColor colour(s);

                // 只能先用if/else了，switch也不能用
//                DiagramItem::DiagramType type;
                QString compName = QString::fromStdString(tagName);
//                qDebug() << "读取到组件名: " << compName;
                if(compName.compare("INPUT") == 0)
                {
                    scene->setIsExistInput(true);
                }
//                emit send2AppOutput(QStringLiteral("读取到组件名： ") + compName,1);
                if(!ui->listWidget->nameList.contains(compName))
                {
                    qDebug() << "读取中发现组件库中缺少组件，组件" << compName << "已被删除";
                    // 使用自定义弹窗无法显示
//                    Utils::alert(QApplication::desktop()->screen()->rect(),"读取出错，缺少组件，组件"+compName+"已被删除");
                    QMessageBox::warning(nullptr, "提示", "组件库中缺少组件\"" + compName + "\"，可能已被删除。请修改场景并重新保存以消除此提示！", QMessageBox::Ok);

                    // 将该元素在场景中删除
                    temp = m.nextSibling();
                    itemNode.removeChild(m);
                    QDomNode arrNode = doc.elementsByTagName("Arrs").at(0);
                    QDomNode a = arrNode.firstChild();
                    QDomNode tt;
                    while(!a.isNull())
                    {
                        tt.clear();
                        if(a.isElement())
                        {
                            QDomElement do_ = a.toElement();
                            if(!do_.attribute("start_item_id").compare(id) ||
                                    !do_.attribute("end_item_id").compare(id))
                            {
                                tt = a.nextSibling();
                                arrNode.removeChild(a);
                            }
                        }
                        if(!tt.isNull())
                        {
                            a = tt;
                        }
                        else
                        {
                            a = a.nextSibling();
                        }

                    }

                    // 删除当前雷达组件空间的xml文件
                    int len = xmlname.split("/").last().length();
                    QString dir_ = xmlname.mid(0, xmlname.length() - len);;
                    qDebug() << "自动获取的文件夹路径：" << dir_;
                    bool delsuc = Utils::deleteXmlFileByName(dir_ + "/room/algoXml/", id.mid(1, id.length() - 2));
                    qDebug() << "删除成功?" << delsuc;
                    qDebug() << "idList展示：" << scene->idList;

                    QString id_split = id.mid(1, id.length() - 2);
                    // 删除代码文件
                    Utils::deleteCodeDirByName(QDir::currentPath() + "/radar/" + getEquip_id() + "/room/", compName + id_split);
                    ui->actionsave->setEnabled(false);
//                    save2XmlFile();
//                    on_actionOpenXml_triggered();
                }
                else
                {
                    DiagramItem *item_ = new DiagramItem(id, compName, scene->getItemMenu(), this->getEquip_id());
//                    DiagramItem *item_ = new DiagramItem(scene->getItemMenu());
                    QPointF pos(posx, poxy);
                    item_->setPos(pos);
                    item_->setBrush(colour);
//                    item_->itemSuuid = id; //id不变
                    item_->callFunction = cf;

                    QDomElement p = e.elementsByTagName("port").at(0).toElement();
                    while(!p.isNull() && !p.tagName().compare("port"))
                    {
                        QString p_id = p.text();
                        QString p_name = p.attribute("name");
                        QString portTemp = p.attribute("isoutput");
                        bool p_isOutput = portTemp.compare("0") == 0 ? 0 : 1;
                        if(portTemp.compare("2") == 0)
                        {
                            p_isOutput = false;
                        }
                        int p_port_type = p.attribute("port_type").toInt();
                        QString p_data_type = p.attribute("dataType");
                        item_->addPort(p_name, p_id, p_isOutput, p_port_type, p_data_type);
                        p = p.nextSiblingElement();
                    }
                    scene->idList.append(id);

                    // 读取xml文件的时候，也要恢复子空间的algorithm
                    QString fullpath = QDir::currentPath() + "/radar/" + getEquip_id() + "/room/algoXml/" + compName + id.mid(1, id.length() - 2) + ".xml";
                    AlgorithmComp ac = Utils::readPluginXmlFile(fullpath);
                    // 加载场景中所有的算法组件
                    scene->add2Scene_comps(id, ac);

                    qDebug() << "加载scene的id列表" << scene->idList << "组件名：" << compName;
                    qDebug() << "scene comps打印" << scene->getScene_comps().keys();
//                    emit send2AppOutput(QStringLiteral("组件名： ") + compName,1);
                    scene->addItem(item_);
                    itemInserted(item_);
                    //更新xml
                    scene->modifyXmlItems(pos, item_);
                    connect(item_, &DiagramItem::showItemsProperties, this, &MainWindow_Radar::receiveItemsid2showProperties);

                    //点击组件显示队列信息
                    connect(item_, &DiagramItem::show_comp_lineinfo, this, &MainWindow_Radar::receiveIteminformation);
                }

            }
            if(temp.isNull())
            {
                m = m.nextSibling();
            }
            else
            {
                m = temp;
            }
        }
        // 大的标签是Arrs的时候
        QDomNode arrowNode = doc.elementsByTagName("Arrs").at(0);
        // 就是arrow了，因为箭头就一种
        QDomNode m1 = arrowNode.firstChild();
        QString start_item_id, end_item_id, arrow_id;

        // 遍历所有的箭头
        while(!m1.isNull())
        {
            if(m1.isElement())
            {
                // 每个元素item
                QDomElement e = m1.toElement();
                arrow_id = e.attribute("id");
                start_item_id = e.attribute("start_item_id");
                end_item_id = e.attribute("end_item_id");
                QString cs = e.elementsByTagName("color").at(0).toElement().text();
                //qDebug() << "箭头颜色： " << cs;
                QColor line_colour(cs);

                QDomElement sp = e.elementsByTagName("start_port").at(0).toElement();
                QDomElement ep = e.elementsByTagName("end_port").at(0).toElement();
                QString sp_id = sp.text();
//                QString sp_name = sp.attribute("name");

                QString ep_id = ep.text();
//                QString ep_name = ep.attribute("name");


                DiagramItem *startItem = getDiagramItemById(start_item_id);
                DiagramItem *endItem = getDiagramItemById(end_item_id);

                ItemPort *sport = startItem->getMports().take(sp_id);

                ItemPort *eport = endItem->getMports().take(ep_id);

                Arrow *arrow = new Arrow(startItem, endItem);
                arrow->setColor(line_colour);
                arrow->itemId = arrow_id; // id不变

                arrow->setPort1(sport);
                arrow->setPort2(eport);
                arrow->setPos1(sport->scenePos());
                arrow->setPos2(eport->scenePos());
                arrow->updatePath();

                scene->idList.append(arrow_id);
//                qDebug() << "scene的id列表" << scene->idList;

                startItem->addArrow(arrow);
                endItem->addArrow(arrow);
                arrow->setZValue(-1000.0);
                connect(arrow, &Arrow::deletearrow_signal, scene, &RadarScene::deal_arrowdelte);
                scene->addItem(arrow);
//                arrow->updatePosition();

                scene->modifyXmlArrows(arrow, startItem, endItem);
            }
            m1 = m1.nextSibling();
        }
        // 更新一下xml文件，当然不执行这一句， 用户编辑之后手动保存也可以
        // 如果用户打开立即关闭，再打开还是会报xx组件已被删除的提示，影响不大
//        isSelectPath = true;
//        save2XmlFile(false);
    }
    else
    {
        // 文件名为空，啥也没选，提示
        Utils::alert(QApplication::desktop()->screen()->rect(), "文件名为空！请重新选择！");
    }
}

/**
 * @brief 通过id找到场景中某个组件
 * @param item_id 算法组件的id
 * @return 找到的item或者空指针
 */
DiagramItem *MainWindow_Radar::getDiagramItemById(QString item_id)
{
    QList<QGraphicsItem *> items = scene->items();
    DiagramItem *im = nullptr;
    for (int i = 0; i < items.size(); i++)
    {
        if((im = dynamic_cast<DiagramItem*>(items.at(i))))
        {
            if(item_id == im->itemSuuid)
            {
                //qDebug() << "找到了";
                return im;
            }
        }
    }
    // 如果没找到，理论上不会。。。
    return nullptr;
}

//触发保存
void MainWindow_Radar::on_actionsave_triggered()
{
    save2XmlFile();
    ui->actionRunRadar->setEnabled(false);//防止调用save2XmlFile()代码部分的ui->actionRunRadar->setEnabled(true)被调用
    // Antrn: 暂时不可用
//    ui->actiongene->setEnabled(true);
}

// weishi:点击场景中的组件时触发，可以在队列信息里查看组件的信息
void MainWindow_Radar::receiveIteminformation(QVariant variant)
{
    comp_info compinfo = variant.value<comp_info>();
    AlgorithmComp ap = this->scene->getScene_comps().take(compinfo.uuid);
    //qDebug() << "底部队列检测窗口：当前组件的中文名是：" << compinfo.cn_name<<"itemsuuid"<<compinfo.uuid;
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem(ap.getInfo().take("Name")));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem(ap.getInfo().take("CN_Name")));

}

// 保存为xml文件
// TODO 新建线程保存文件
bool MainWindow_Radar::save2XmlFile(bool tip)
{
    //Utils::alert(qApp->desktop()->rect(), "请确保组件连接完毕!");
#if 0
    //保存场景信息的时候需要确定场景中是否已经包括输入输出组件
    //如果不存在需要提示用户
    if(!(isInputExist && isOutputExist))
    {
        Utils::alert(qApp->desktop()->rect(), "输入输出组件不全!");
        emit send2AppOutput(QStringLiteral("[警告]输入输出组件不全!"), 0);
    }
#endif
    ui->statusbar->showMessage("正在保存场景...", 1000);
    // 根元素
    QDomElement rootElem = scene->getDoc()->documentElement();
    rootElem.setAttribute("width", scene->width());
    rootElem.setAttribute("height", scene->height());
    //[更新箭头和组件的color
    DiagramItem *d;
    Arrow *a;
    // 遍历场景中的items
    for (int i = 0; i < scene->items().size(); i++)
    {
        //转换为DiagramItem成功，只算item，排除箭头
        if((d = dynamic_cast<DiagramItem*>(scene->items().at(i))))
        {
            //                qDebug() << d->brush().color().name();
            // elementById()报错说没有实现
            //                if(d->brush().color().name() != scene->getDoc().elementById(QString::number(d->itemId)).firstChild().toElement().text()){
            //                    scene->getDoc().elementById(QString::number(d->itemId)).firstChild().setNodeValue(d->brush().color().name());
            //                }
            QDomNodeList list = scene->getDoc()->elementsByTagName("Items").at(0).childNodes();
            for (int k = 0; k < list.count(); k++)
            {
                QDomElement e = list.at(k).toElement();
                //                    qDebug() << e.attribute("id") << "; " << QString::number(d->itemId);
                //找到id对应的元素
                if(e.attribute("id").compare(d->itemSuuid) == 0)
                {
                    //                        e.firstChild().setNodeValue(d->brush().color().name());
                    //                        qDebug() << d->brush().color().name();
                    //                        qDebug() << e.elementsByTagName("color").at(0).toElement().text();
                    e.elementsByTagName("color").at(0).toElement().firstChild().setNodeValue(d->brush().color().name());
                    e.elementsByTagName("call").at(0).toElement().firstChild().setNodeValue(d->callFunction);
                }
            }
        }
        // 如果是箭头
        if((a = dynamic_cast<Arrow *>(scene->items().at(i))))
        {
            QDomNodeList list = scene->getDoc()->elementsByTagName("Arrs").at(0).childNodes();
            for (int k = 0; k < list.count(); k++)
            {
                QDomElement e = list.at(k).toElement();
                //找到id对应的元素
                if(e.attribute("id").compare(a->itemId) == 0)
                {
                    e.elementsByTagName("color").at(0).toElement().firstChild().setNodeValue(a->getColor());
                }
            }
        }
    }
    //]更新color

    // 保存雷达组件数据，文件放到此雷达命名的文件夹中，要保证文件夹存在
    QString dirp = QDir::currentPath() + "/radar/" + getEquip_id() + "/";
    QRect rect = QApplication::desktop()->screen()->rect();
    // 如果之前自己选择位置存错过
    if(isSelectPath)
    {
        switch (Utils::saveFile(this, dirp, getEquip_id() + ".rad", scene, false, userSelectPath))
        {
        case 1:
            if (tip)
            {
                ui->statusbar->showMessage("保存成功", 1000);
                Utils::alert(rect, "场景保存成功!");
            }
            break;
        case -1:
            if (tip)
            {
                ui->statusbar->showMessage("保存出错", 1000);
                Utils::alert(rect, tr("场景保存出错，请重新尝试!"));
                return false;
            }
            break;
        case 0:
            if (tip)
            {
                ui->statusbar->showMessage("保存失败", 1000);
                Utils::alert(rect, tr("场景保存失败！请选择存储位置后再保存!"));
                return false;
            }
            break;
        }
    }
    else
    {
        // NOTE 弹窗用户选择存储路径， 用户不能自定义文件名
        switch (Utils::saveFile(this, dirp, getEquip_id() + ".rad", scene, true, userSelectPath))
        {
        case 1:
            ui->statusbar->showMessage("保存成功", 1000);
            Utils::alert(rect, "场景保存成功!");
            break;
        case -1:
            ui->statusbar->showMessage("保存出错", 1000);
            Utils::alert(rect, tr("场景保存出错，请重新尝试!"));
            return false;
        case 0:
            ui->statusbar->showMessage("保存失败", 1000);
            Utils::alert(rect, tr("场景保存失败！请选择存储位置后再保存!"));
            return false;
        }
        isSelectPath = true;
    }

    // TODO 是否需要快照应该由用户决定，后期需要完善
//     saveSnapshot(1); //保存场景快照
    // saveSnapshot(2); //保存视图快照
    isSave = true;
    toggleSaveXml(0);
//    ui->actionRunRadar->setEnabled(true);
//    QMessageBox qm;
//    qm.setText(tr("场景保存成功!"));
//    qm.exec();
//    u->alert(geometry(), tr("场景保存成功!"));
    return true;
}

/**
 * @brief 打开自动读取已有的xml文件,雷达rad文件
 * @param id 雷达名字
 */
void MainWindow_Radar::autoloadXmlById(QString id)
{
    // 遍历文件夹
    QFileInfoList list = getFileList(QDir::currentPath() + "/radar/" + getEquip_id() + "/");
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        QString fname = fileInfo.fileName();
        // 通过雷达id筛选文件
        if(fname.startsWith(id))
        {
            QString filepath;
            filepath.append(fileInfo.path());
            filepath += "/" + fname;
            qDebug() << "找到rad!" << filepath;
            emit send2AppOutput(QStringLiteral("[提示]：找到雷达.rad文件! ") + filepath, 1);
            readXmlConf(filepath);
            isSave = true;
            toggleSaveXml(0);
            qDebug() << "自动读取，不算是手动保存的操作，所以isSave恢复已保存状态";
//            emit send2AppOutput(QStringLiteral("自动读取，不算是手动保存的操作，所以恢复已保存状态"),1);
            break;
        }
    }
}

/**
 * @brief 获取文件夹中所有的文件
 * @param path 文件夹路径
 * @return
 */
QFileInfoList MainWindow_Radar::getFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = getFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}

// 进度条相关
// 当已经开始运行的时候
void MainWindow_Radar::On_start()
{
    // 关闭几个按钮和动作
//    ui->actionRunRadar->setEnabled(false);
    // 打开停止按钮和动作
//    ui->action_Stop->setEnabled(true);
    // TODO 主线程加入显示UI线程
    ui->statusbar->addWidget(progressBar);
    // 进度条为0
    progressBar->setValue(0);
//    ui->statusbar->showMessage(label_time->text(), 1000);
}

// 当运行结束的时候
void MainWindow_Radar::On_over()
{
//    ui->action_Stop->setEnabled(false);
    ui->actionRunRadar->setEnabled(true);
//    label_time->setText(tr("Done"));
    ui->statusbar->removeWidget(progressBar);
}

// 当对信号进行评估进度的时候，实时设置进度条的值
void MainWindow_Radar::On_rateSignal(float rate)
{
    progressBar->setValue(int(rate));
}

// 当isSave标志变为false时就把保存xml按钮置为可点击
void MainWindow_Radar::On_isSave2False(QString message)
{
    // 运行时未保存，需要弹窗
    if(!message.isNull() && !message.isEmpty())
    {
        int res = QMessageBox::question(this, tr("提示"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(res == QMessageBox::Yes)
        {
            qDebug() << "save";
            save2XmlFile();
            isSave = true;
            toggleSaveXml(0);
            ui->actionRunRadar->setEnabled(true);
            return;
        }
        else if(res == QMessageBox::No)
        {
            qDebug() << "do not save";
            // 直接退出
        }
        else
        {
            // 拒绝关闭
            qDebug() << "拒绝关闭!!!";
        }
    }
    ui->actionsave->setEnabled(true);
}

/**
* @projectName   prototype_v0906
* @brief         简介 动态更新右面属性列表，内容都是可编辑的。但是问题是怎么获取用户编辑的是哪一个呢？
* @author        Antrn
* @date          2019-10-04
*/
void MainWindow_Radar::update_Comp_property(AlgorithmComp ac)
{
    toShowPropertiesDock(ac);
}


//void MainWindow_Radar::setComp_typeandMode(int id)
void MainWindow_Radar::setComp_typeandMode(QString iconName, RadarScene::Mode mode)
{
//    scene->setItemType(DiagramItem::DiagramType(id-1));
    scene->setMyItemIconName(iconName);
    scene->setMode(mode);
}

/**
 * @brief 搜索组件，找到就显示一个，否则就全部显示
 */
void MainWindow_Radar::search()
{
    QString strText = m_pSearchLineEdit->text();
    if (!strText.isEmpty())
    {
//        QMessageBox::information(this, QStringLiteral("搜索"), QStringLiteral("搜索内容为%1").arg(strText));
        ui->listWidget->clear();
        loadCompByName(strText);
    }
    else
    {
        loadAllComps();
    }
}

/**
 * @brief 搜索框的自动补全
 */
void MainWindow_Radar::editComplete()
{
    QString text = m_pSearchLineEdit->text();
    if(QString::compare(text, QString("")) != 0)
    {
        bool is_contains = word_list.contains(text, Qt::CaseInsensitive);
        if(!is_contains)
        {
            word_list << text;
            string_list_model->setStringList(word_list);
            //completer->setModel(new QStringListModel(wordList, this));
        }
    }
}

/**
 * @brief 获取id
 * @return id
 */
QString MainWindow_Radar::getEquip_id() const
{
    return equip_id;
}

void MainWindow_Radar::setEquip_id(const QString &value)
{
    equip_id = value;
}

//左边组件窗口显示与隐藏
void MainWindow_Radar::on_actionCom_list_triggered()
{
    //显示工具箱（文本编辑器）
    if(ui->dockCompList->isVisible())
    {
        ui->dockCompList->close();
    }
    else
    {
        ui->dockCompList->show();
    }
}

//左下角状态栏显示XY
void MainWindow_Radar::xy_show(double x, double y)
{
    QString str = QString("x=%1,y=%2").arg(QString::number(x)).arg(QString::number(y));
    label_xy->setText(str);
}

MainWindow_Radar::~MainWindow_Radar()
{
//    SysMessage::close_sys_message();
    delete ui;
}

/**相当于一个槽函数 所以想要改变窗口的状态 直接改变actionproperty最好
 * @brief 属性窗口的显示与关闭
 * @param checked
 * setChecked 会触发
 */
void MainWindow_Radar::on_actionproperty_toggled(bool checked)
{
    if(checked)
    {
        ui->dockWidget->show();
    }
    else
    {
        ui->dockWidget->hide();
    }
}
/**
 * @brief 接收其他操作传递的信号，将消息显示在调试窗口中
 * @param message 消息信息
 */
void MainWindow_Radar::receiveFromSend(QString message, int flag)
{
    if(flag == 0)
    {
        ui->textEdit->setTextColor(QColor(255, 0, 0));
    }
    ui->textEdit->append(message);//添加字符串作为一个段落到TextEdit控件中
}

/**
 * @brief 接收到item传过来的算法id，展示出来，不是只读
 * @param sid
 */
void MainWindow_Radar::receiveItemsid2showProperties(QString sid)
{
    AlgorithmComp ap = this->scene->getScene_comps().take(sid);
    this->toShowPropertiesDock(ap, false);
}

void MainWindow_Radar::generateIcon(QString icon_name)
{
    Utils::generateIcon(icon_name, getEquip_id());
}

/**
 * @brief MainWindow_Radar::codeEditSlot 编辑场景中组件代码
 */
void MainWindow_Radar::codeEditSlot()
{
    // 将当前场景中的组件传入，编辑它的代码
//    CodeWindow *cw = new CodeWindow(this, nullptr, scene->selectedItems().first());
//    cw->show();
    // Antrn: 启用少华分离的代码生成模块
    QGraphicsItem *item = scene->selectedItems().first();
    DiagramItem *item_ = nullptr;
    item_ = qgraphicsitem_cast<DiagramItem *>(item);
    AlgorithmComp ac = this->scene->getScene_comps().value(item_->itemSuuid);
    this->main_process->getComponent_process()->codeEdit(nullptr, item, item_, &ac);
}

void MainWindow_Radar::on_tabWidget_2_destroyed()
{
    qDebug() << "测试tab destroyed";
//    ui->dockWidget_3->setEnabled(false);
}

void MainWindow_Radar::on_tabWidget_2_tabBarClicked(int index)
{
    qDebug() << "测试tab clicked";
//    ui->dockWidget_3->close();
}

/**
 * @brief MainWindow_Radar::on_tabWidget_2_tabCloseRequested
 * @param index
 */
void MainWindow_Radar::on_tabWidget_2_tabCloseRequested(int index)
{
    qDebug() << "index:" + QString::number(index) + " 测试tab closed";
}

/**
 * @brief click the generate data button to pop up window
 */
void MainWindow_Radar::on_actiongene_triggered()
{
    qDebug() << "正在生成雷达仿真数据...";
    SimDataGenWidget *sdgw = new SimDataGenWidget(this->getEquip_id(), nullptr);
//    sdgw->setWindowModality(Qt::WindowModal);
    sdgw->show();
    // user can click the button of generate codes
    ui->actionGeneCode->setEnabled(true);
}

/**
 * @brief MainWindow_Radar::on_actionGeneCode_triggered
 * 当用户点击生成代码之后会根据场景中的算法组件列表向.c文件中write codes, 按照箭头的顺序
 * 添加算法组件的函数调用
 */
void MainWindow_Radar::on_actionGeneCode_triggered()
{
    // 当前雷达的最后的main代码文件
    // QString radar_code_file = QDir::currentPath() + "/radar/" + this->getEquip_id() + "/room/" + this->getEquip_id() + ".c";

    // Antrn: index 相同的放一起
    QList<DiagramItem*> diagramlist;

    // Antrn: 一开始设计，包括输入/显示/输出组件的组件列表
    QList<DiagramItem*> diagramlistaddInputOutputDisplay;

    // Antrn: 将场景中的算法列表diagramlist按照index从小到大排序
    addAndsortDiagramList(&diagramlist, this->scene->items());

    // Antrn: 计算除了输入输出组件加显示组件的数量
    int nums_comp = 0;

    // Antrn: 将输入输出和显示组件从diagramlist中剔除
    for ( DiagramItem * i : diagramlist)
    {
        diagramlistaddInputOutputDisplay.append(i);
        // Antrn: 2020713增加输入输出显示组件判断
        if(i->iconName.compare("INPUT") != 0 && i->iconName.compare("OUTPUT") != 0 && i->iconName.compare("display") != 0)
        {
            qDebug() << "是算法组件，保留" << i->iconName << i->index;
            nums_comp++;
        }
        else
        {
            diagramlist.removeOne(i);
            qDebug() << "是输入/输出/显示组件, 删除";
        }
    }
//    qDebug() << "看一下不加INPUT和OUTPUT/Display以及加上这两个的列表的大小：" << diagramlist.size() << "<--->" << diagramlistaddInputOutputDisplay.size();
    // 保存当前组件(不包括箭头和输入输出显示组件)的数量
    this->scene->setCompNums(nums_comp);

    // 用来表示index的值
    int iid = 1;
    // DEPRECATED
    // 20200628 去掉输入输出组件, 因为输入是1, 输出是最大的index，已经在上一步剔除过，不考虑输出的index
    // int iid = 2;

    // Antrn: 先清空
    scene->diagramListByindex.clear();

    // Antrn: 遍历剔除输入输出组件后的列表
    for(int p = 0; p < diagramlist.size(); p++)
    {
        QList<QString> tempD;
        for(int r = 0; r < diagramlist.size(); r++)
        {
            DiagramItem *d = diagramlist.at(r);
            if(d->index == iid)
            {
                tempD.append(d->getIconName() + d->itemSuuid);
            }
        }
        iid++;
        if(tempD.size() > 0)
        {
            scene->diagramListByindex.append(tempD);
        }
        else
        {
            break;
        }
    }
    // Antrn: 顺序输出测试
    qDebug() << "[test]展示验证执行顺序是否正确";
    for(int i = 0; i < scene->diagramListByindex.size(); i++)
    {
        for(int j = 0; j < scene->diagramListByindex.at(i).size(); j++)
        {
            qDebug() << scene->diagramListByindex.at(i).at(j);
        }
        qDebug() << "-------------------index" << i + 1 << "不同的分割线--------------------------";
    }

    // Antrn: 一定要先清空列表！
    scene->orderCompSidList.clear();
    for(auto di : diagramlist)
    {
        scene->orderCompSidList.append(di->getIconName() + di->itemSuuid);
    }

    // Antrn: 先生成Main.cpp
//    QString mainCppPath = QDir::currentPath() + "/radar/" + this->getEquip_id() + "/room/Main.c";
//    Utils::generateMain(&diagramlistaddInputOutputDisplay, mainCppPath);

    //shao:新的代码生成 代码生成的主流程写在了 main_process 这个类里面，不继续在Utils里面写了，为了分清模块，如果有问题可以定位到单个小类，缩小检查范围，
    //所以就不用 Utils::generateMain这个方法了也不用Utils::codeGenerate这个方法了 新的生成方法  copy了他们的代码
    QString mainCodePath = QDir::currentPath() + "/radar/" + this->getEquip_id() + "/room/codes/";
    bool is_cg = main_process->code_generate(&diagramlist, mainCodePath);
    if(is_cg)
    {
        Utils::alert(qApp->desktop()->rect(), "代码生成完毕，请前往目录查看");
        main_process->package(&diagramlist, mainCodePath);
        ui->action_compile->setEnabled(true);
        ui->action_sendCode->setEnabled(true);
    }
    else
    {
        qDebug() << "代码生成失败！无法进行下一步打包操作，请检查";
    }
#if 0
    // Antrn: 2020-10-17 暂时不需要生成一个大的main文件了

    // Antrn: 使用工具类的方法生成代码文件
    Utils::codeGenerate(&diagramlistaddInputOutputDisplay, this, radar_code_file);

    // Antrn: copy to "Main" directory  shao:???
    QString distFile = "/home/mao/Desktop/Main/main_test.c";
    if(Utils::qCopyFile(radar_code_file, distFile))
    {
        qDebug() << "file copied successfully to: " << distFile;
    }
    else
    {
        qDebug() << "file copied failed to: " << distFile << "maybe already existed. please check it.";
    }

    // Antrn: 生成generate.c文件
    Utils::generateDataCodeFile(diagramlist, getEquip_id());

    QMessageBox::information(nullptr, "提示", "generate.c和main主代码生成完成！");
    // 代码预览与编辑, init和.h文件就先不管，主要负责c文件
    CodePreview *editor = new CodePreview(radar_code_file, nullptr);
    editor->setWindowTitle(QObject::tr("生成代码预览和修改"));
    editor->show();
#endif
}

/**
 * @brief MainWindow_Radar::on_action_run_triggered
 * 运行就绪后的运行代码
 */
void MainWindow_Radar::on_action_run_triggered()
{
    // 自己写代码的时候测试用的，只是执行一下当前生成的这个.c文件，是按照radar.c拼接的，但是头文件什么的有缺失
//    QString workpath = QDir::currentPath()+"/radar/"+this->getEquip_id()+"/room/";
//    QuDong::startRunCpp(workpath+this->getEquip_id()+".c", workpath+"/"+this->getEquip_id(), this->getEquip_id());

    // Antrn: TODO 调用执行函数
    qDebug() << "这里是执行运行函数";

#if 0
    QList<QString> nameidlist;
    // Antrn: 某个设备空间中的所有组件的数据文件目录
    QDir wave_dir(QDir::currentPath() + "/radar/" + equip_id + "/room/waveData/");
//    qDebug() << wave_dir.absolutePath();
    // 要判断路径是否存在
    if(!wave_dir.exists())
    {
        qDebug() << "it is not true path";
        return ;
    }
    // 实现对文件夹的过滤
    wave_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    QFileInfoList list = wave_dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        nameidlist.append(fileInfo.fileName());
    }


    // 0501版本 202007去扬州的代码
    QuDong qd(nameidlist, equip_id);
    // Antrn: 新思路：把上面生成的.c程序复制到/Projects/test/code下，去执行就OK了，这样就有一系列头文件，满足运行需求
//#if 0
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd(qAppName() + "/Projects/test/code");
    // 如果是打包成可执行文件，则需要这样写
//#endif
    QDir dir(QDir::currentPath() + "/Projects/test/code");
    qDebug() << "在此目录运行代码：" << dir;
    QString workpath = dir.absolutePath();
    qd.startRun(workpath + "/" + getEquip_id() + ".c", workpath + "/" + getEquip_id() + ".out", "./" + getEquip_id() + ".out");
#endif
}

/**
 * @brief MainWindow_Radar::on_action_sendCode_triggered
 * 点击分发代码，向服务器进行分发，关于分发的顺序还有待商讨，应该是系统自动判别服务器的状态，进行自适应分配
 * 或者是根据算法组件的数量循环依次分配
 */
bool MainWindow_Radar::on_action_sendCode_triggered()
{
    // 检测服务器的可用状态，根据可用状态提供服务器的地址，这些暂时默认服务器都是正常状态的
    // 数量也是固定的，所以只需要读取服务器的IP地址列表，然后将已有的算法组件循环顺次往下传
    // 传完后返回一个map，就是服务器和算法组件的映射（算法组件数量大于服务器的数量怎么办），那就
    int cnt_comp = this->scene->getCompNums();
    // 记录下id和组件名的映射图
//    QMap<QString, QString> id_name_map;
    // key是服务器ip，后面是组件的列表，String是算法组件名和id的拼接，每个服务器对应分配一个列表
    QMap<QString, QList<QString>> server_comp_map;
    // 这个是用来表示顺序，直接将组件和服务器IP对应起来
    QList<QString> server_comp;
    // 服务器列表
    QList<QString> serverList = Utils::getServerList();
    // 暂时可以是固定的数目
    int cnt_server = Utils::getServerNums();
    // 没有可用服务器
    if(cnt_comp == 0)
    {
        qDebug() << "场景中没有组件或者用户还没先点击生成代码！";
        return false;
    }
    else
    {
//        qDebug() << "orderCompSidList" << this->scene->orderCompSidList;
        // key 是组件的id
//        for(auto i: this->scene->orderCompSidList){
//            id_name_map.insert(i, scene->getScene_comps().value(i).getFileName());
//        }
//        qDebug() << cnt_comp << "->两种计算组件数量对比" << id_name_map.size();
//        QList<QString> nameAddIdStr(this->scene->orderCompSidList);
        // 记录总的组件拼接
//        for(auto id : id_name_map.keys()){
//            // name+id拼接
//            nameAddIdStr.append(id_name_map.value(id)+id);
//        }
//        qDebug() << "algoNameAddIdStr，按照index粗略拍了顺序" << nameAddIdStr;

        // 组件数大于服务器
        if(cnt_comp > cnt_server)
        {
            // 遍历服务器的数量
            for(int i = 0; i < cnt_server; i++)
            {
                // 每次新建一个列表，计算要存放在当前服务器的算法组件
                QList<QString> tempComp_nameId;
//                for(int j=i;j<cnt_comp;j+=cnt_server){
//                    tempComp_nameId.append(nameAddIdStr.at(j));
//                }
                for(int j = i; j < scene->diagramListByindex.size(); j += cnt_server)
                {
                    QList<QString> td = scene->diagramListByindex.at(j);
                    for(int w = 0; w < td.size(); w++)
                    {
                        tempComp_nameId.append(td.at(w));
                    }
                }
                server_comp_map.insert(serverList.at(i), tempComp_nameId);
            }
            qDebug() << "[服务器不足时分配组件情况:]" << server_comp_map;
            int index = 0;
//            for(auto n: nameAddIdStr){
//                server_comp.append(n+"\t"+serverList.at(index%cnt_server));
//                index++;
//            }
            for(auto n : scene->diagramListByindex)
            {
                for(auto m : n)
                {
                    server_comp.append(m + "\t" + serverList.at(index % cnt_server));
                }
                index++;
            }
        }
        else
        {
#if 0
            // 当服务器用不完的时候，是不是还得记录哪一台被使用，哪些没用到
            for(int i = 0; i < cnt_comp; i++)
            {
                // 每次新建一个列表，计算要存放在当前服务器的算法组件
                QList<QString> tempComp_nameId;
                tempComp_nameId.append(nameAddIdStr.at(i));
                server_comp_map.insert(serverList.at(i), tempComp_nameId);
            }
            qDebug() << "[服务器数量充足的情况下分配情况：]" << server_comp_map;
//            qDebug() << this->scene->getScene_comps().keys();
            for(int i = 0; i < cnt_server; i++)
            {
                server_comp.append(nameAddIdStr.at(i) + "\t" + serverList.at(i));
            }
#endif
            for(int i = 0; i < scene->diagramListByindex.size(); i++)
            {
                // 每次新建一个列表，计算要存放在当前服务器的算法组件
                QList<QString> tempComp_nameId;
                for(auto x : scene->diagramListByindex.at(i))
                {
                    tempComp_nameId.append(x);
                }
                server_comp_map.insert(serverList.at(i), tempComp_nameId);
            }
            qDebug() << "[服务器数量充足的情况下分配情况：]" << server_comp_map;
//            qDebug() << this->scene->getScene_comps().keys();
//            for(int i=0;i<cnt_server;i++){
//                server_comp.append(nameAddIdStr.at(i)+"\t"+serverList.at(i));
//            }
            for(int i = 0; i < cnt_comp; i++)
            {
                for(auto h : scene->diagramListByindex.at(i))
                {
                    server_comp.append(h + "\t" + serverList.at(i));
                }
            }
        }
    }

    QString filePath = QDir::currentPath() + "/radar/" + getEquip_id() + "/";

    // 写入文件server.txt
    QFile file(filePath + "server.txt");
    //
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    file.write("# 按照服务器IP地址排序：\n");
    // si: server IP
    for(auto si : server_comp_map.keys())
    {
        for(auto sname_id : server_comp_map.value(si))
        {
            file.write((si + "\t" + sname_id + "\n").toUtf8());
        }
    }
    file.write("\n# 按照组件执行顺序排序（可以参考，但不能使用，因为并行的两个组件在这里也有先后了）：\n");
    for(auto sc : server_comp)
    {
        file.write((sc + "\n").toUtf8());
    }
    file.close();

    // Antrn: 这里启动DCPS
    if(!Utils::startDCPS())
    {
        qDebug() << "启动失败";
    }
    else
    {
        qDebug() << "启动成功";
    }

//    p->waitForReadyRead();
//    p->waitForFinished();
//    qDebug() << "Process State: " << p->state()  << p->readAll() << "and transport() not start...";
//    p->startDetached("sh", args, QIODevice::ReadWrite);


//    p->waitForStarted();
//    p->waitForFinished();
//    QString strTemp = QString::fromLocal8Bit(p->readAllStandardOutput());
//    QString strError = QString::fromLocal8Bit(p->readAllStandardError());
//    p->close();
//    p->kill();
//    qDebug() << "Process State: " << p->state() << "and transport() start...";


    // Antrn: FIXME RPC Xml 路径问题要修改
//    QString rpcfp = "/home/a913/BRPC/example/streaming_echo_c++/";
    QString rpcfp = QDir::currentPath() + "/radar/" + getEquip_id() + "/";

    QString rpcfn = "rpc.xml";
    // 服务器列表
    QList<QString> sl;
    QList<QString> fromps;
    QList<QString> tops;
    QList<QString> shpkg;
    // 这个cnt_comp问题很大，当连线并行的时候，相同index权重的算法应该放在同一台服务器上
    // Antrn: 暂时注释掉这两个循环，跟王源老哥暂时约定生成一个node即可
#if 0
    for(int i = 0; i < cnt_comp; i++)
    {
        fromps.append("../Main/");
        tops.append("/home/mao/");
    }
    sl.clear();
    for(int j = 0; j < cnt_server; j++)
    {
        // 当一台服务器上药运行多个算法组件时候怎么办
        for(QString al : server_comp_map.value(serverList.at(j)))
        {
            shpkg.append(al + ".sh");
        }
        for(int k = 0; k < server_comp_map.value(serverList.at(j)).size(); k++)
        {
            sl.append(serverList.at(j));
        }
    }
#endif
//    fromps.append("./radar/" + getEquip_id() + "/room/codes/target/");
    fromps.append("./target/");
    tops.append("../../../");
    // 暂时就一个节点
    sl.append(serverList.at(0));
    shpkg.append("FDPC2.sh");

    bool rpctof = Utils::generateXml4RPC(rpcfp, rpcfn, sl, fromps, tops, shpkg);

    if(rpctof)
    {
        Utils::alert(qApp->desktop()->rect(), "RPC XML生成成功");
    }
    else
    {
        qDebug() << "RPC Xml写入+Copy is successful? ---->" << rpctof;
        QMessageBox::warning(this, "异常提醒", tr("RPC XML生成失败"));
    }

    // Antrn: windows平台先注释，在银河麒麟上测试通过
//    transport();

    // DDS Xml
    QString ddsfp = filePath;
    // 总共流程执行的步骤数
    int s_ = scene->diagramListByindex.size();

    // 遍历服务器就行
    for(int i = 0; i < cnt_server; i++)
    {
        QString ddsfn = serverList.at(i) + ".xml";
        int cnt;
        // 保持存入时的顺序
        QList<QString> algos;
        // 当前服务器上总共要运行的算法组件数组
        QList<QString> tsa = server_comp_map.value(serverList.at(i));
        cnt = tsa.size();
        // 存放当前算法组件的后面的算法组件
        QMap<QString, QList<QString>> nextAlMap;
        for(auto al : tsa)
        {
            algos.append(al);
            // 遍历一次步骤，找到当前算法组件在哪一步
            for(int y = 0; y < s_; y++)
            {
                if(scene->diagramListByindex.at(y).contains(al) && y + 1 < s_)
                {
                    QList<QString> nextAls;
                    // 将下一个步骤需要的组件id传入数组
                    for(auto e : scene->diagramListByindex.at(y + 1))
                    {
                        nextAls.append(e);
                    }
                    nextAlMap.insert(al, nextAls);
                    break;
                }
                else
                {
                    qDebug() << "当前下标" << y << "中没有此组件" << al << "或者是最后一个组件了";
                }
            }

        }
        bool ddsTof = Utils::generateXml4DDS(ddsfp, ddsfn, serverList.at(i), "123456", cnt, algos, nextAlMap);
//        if(ddsTof){
//            Utils::alert(qApp->desktop()->rect(), "DDS XML生成成功，路径为"+ddsfp);
//        }
        qDebug() << "DDS XML写入" << ddsTof;
    }
    // You can click Compile Button
//    ui->action_compile->setEnabled(true);
//    ui->action_sendCode->setEnabled(false);
#if 0
    // 按照上述的映射，使用system启动RPC，将代码分发，返回一个状态信息，如果分发成功则返回true
    bool tof = Utils::despatchCode(filePath);
    if(tof)
    {
        // 20200724 将生成的代码发送到 Projects/code/下面

//#if 0
        QDir dir(QDir::currentPath());
        dir.cdUp();
        dir.cd(qAppName() + "/Projects/test/code/");
        // 如果是打包成可执行文件，则需要这样写
//#endif
//        QDir dir(QDir::currentPath() + "/Projects/test/code");
        QString newreportpath = dir.absolutePath() + "/" + getEquip_id() + ".c";
        QFileInfo fi(newreportpath); //将要复制到的新文件完整路径
        QString name = fi.fileName();  //获取文件名
        //判断文件按是否存在，存在则删除
        if(fi.exists())
        {
            qDebug() << name << "文件已存在，先删除";
            dir.remove(name);
        }
        if(QFile::copy(QDir::currentPath() + "/radar/" + getEquip_id() + "/room/" + getEquip_id() + ".c", newreportpath)) //将文件复制到新的文件路径下
        {
            qDebug() << QStringLiteral("复制成功");
            return Utils::alert(qApp->desktop()->rect(), "代码分发成功");
        }
        else
        {
            qDebug() << "复制失败";
            return false;
        }
        // 这个方法是使用system启动DDS，建立服务器上的算法组件的订阅发布关系
        bool status = buildDdsPublishSubscribeRelation(server_comp_map, algoCompOrder);
        if(status)
        {
            // 提示用户一切就绪，可以点击执行代码
            showReadyStatus();
        }

    }
    else
    {
//        alert("代码分发错误！");
        return false;
    }
#endif
    return true;
}

// DEL 只是用来最初测试向场景中 添加组件用的
// 实际没有任何用处，不能相互连接，况且都没有组件名
void MainWindow_Radar::on_action_rand_add_triggered()
{
    DiagramItem *b = new DiagramItem(itemMenu, nullptr);

    scene->addItem(b);
    static const char* names[] = {"Vin", "Voutsadfasdf", "Imin", "Imax", "mul", "add", "sub", "div", "Conv", "FFT"};
    for (int i = 0; i < 4 + rand() % 3; i++)
    {
        b->addPort(names[rand() % 10], QUuid::createUuid().toString(), rand() % 2, 0, nullptr);
        // 添加在场景中间
        b->setPos(scene->sceneRect().center().toPoint());
    }
}
/**
 * @brief MainWindow_Radar::on_action_compile_triggered
 * 将代码分发和代码测试放在一起，统称为代码编译
 */
void MainWindow_Radar::on_action_compile_triggered()
{
#if 0
    if(on_action_sendCode_triggered())
    {
        QTimer::singleShot(800, this, [ = ]()
        {
            on_action_run_triggered();
        });
        //如果成功则可以进行运行操作
        //show data plot
        ui->actionRunRadar->setEnabled(true);
        //run code
        ui->action_run->setEnabled(true);
    }
    else
    {
        qDebug() << "失败！！";
    }
#endif
    qDebug() << "this is Compile procedure...";
    // run generate.c

    ui->action_compile->setEnabled(false);
    ui->action_run->setEnabled(true);
}

/**
 * @brief 点击管理服务器节点按钮
 */
void MainWindow_Radar::on_actionmanage_Node_triggered()
{
    qDebug() << "管理服务器";
}


void MainWindow_Radar::on_actionClusterState_triggered()
{
    ClusterStateDialog *clusterStateDialog = new ClusterStateDialog(this);
    clusterStateDialog->show();
}

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-10-15 T 15:37:24
 * @copyright (c)
 * @status developing
 * @brief 主窗口中菜单上的【整体参数配置】按钮触发槽函数
 */
void MainWindow_Radar::on_action_config_triggered()
{
    // Antrn: 调用为将要生成的主函数 整体的配置窗口
    this->main_process->Main_function_configuration(this->scene->getScene_comps());
}
//weishi:
void MainWindow_Radar::deal_isSignalShowOpen()
{
    isSignalShowOpen = false;
    showWaveGraphicAction->setEnabled(true);
}
