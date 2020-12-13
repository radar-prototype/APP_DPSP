#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "realtimecurve.h"
#include "draglistwidget.h"
#include "mainwindownew.h"
#include <QLabel>
#include <QSettings>
#include <QWhatsThis>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include "ecmitem.h"
#include "dataplaybackwidget.h"
#include "utils.h"
#include "simupparamsetting.h"
#include "main_window/main_left_list.h"
#include "page_structure/left_side_switch/left_utils.h"
/**
* @projectName   prototype_v0719
* @brief         整个程序的主窗口： 打开现有的工程时。
* @author        Antrn
* @date          2019-08-21
*/
MainWindow::MainWindow(QString title, QString dpsp_path, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    w_title(title),
    p_dpsp_path(dpsp_path)
{
    ui->setupUi(this);

    // TODO 打开Mainwindow的日志捕获
//    SysMessage::open_sys_message_without_textEdit();
//    qInstallMessageHandler(SysMessage::SysMessageOutput);

    setWindowTitle(w_title);
//    initOpenGL();
    // 关闭时自动释放窗口对象
    setAttribute( Qt::WA_DeleteOnClose, true );
    // default opened toolbox is radar
    ui->toolBox_left->setCurrentIndex(0);
    // 第一次尝试 what's this?
    QAction *whatAction = QWhatsThis::createAction(this);
    ui->mainToolBar->addAction(whatAction);

    // 接收来之左边item的信号
    connect(this->ui->listWidget_rader, &Main_left_list::itemOperate, this, &MainWindow::itemOperateSlot);
    connect(this->ui->listWidget_ecm, &Main_left_list::itemOperate, this, &MainWindow::itemOperateSlot);
    connect(this->ui->listWidget_object, &Main_left_list::itemOperate, this, &MainWindow::itemOperateSlot);

    //shao：合并新建工程添加的代码，如果path为空说明是新建,  就自己新建一个 工程文件夹
    if(QString::compare("",dpsp_path)==0){
       p_dpsp_path=this->new_a_prohect(title);
    }

    // 读取工程文件
    if(loadProject(p_dpsp_path))
    {
        // Qt 5.6.1 报错
//        qDebug() << "加载成功！" << info.toStdMap() << comps.toStdMap();
        foreach(const QString &id, comps.keys())
        {
            QString path;
            // 获取设备的name
            QString item_name=comps.value(id);;
            if(id.startsWith("radar"))
            {
                this->ui->listWidget_rader->add_listWidgetItem(item_name);
//                Store::open_radar_window(item_name+"/");//shao:"/"是应为接受函数默认是文件路径
                Store::open_window(Page_structure_utils::radar,item_name+"/");
            }
            else if(id.startsWith("ecm"))
            {
                this->ui->listWidget_ecm->add_listWidgetItem(item_name);
//                Store::open_radar_window(item_name);
                Store::open_window(Page_structure_utils::ecm,item_name+"/");
            }
            else if(id.startsWith("object"))
            {
                this->ui->listWidget_object->add_listWidgetItem(item_name);
//                Store::open_radar_window(item_name);
                Store::open_window(Page_structure_utils::object,item_name+"/");
            }
        }
    }
    else
    {
        qDebug() << "加载失败！";
//        this->close();
    }
    graphicsScene = new MainWindowNewScene();
    // 将窗口中的场景设置为自定义的场景
    ui->graphicsView->setScene(graphicsScene);
    ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);//这里是左上角方式显示
    // 设置鼠标跟踪开启
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setAcceptDrops(true);
    // 刷新全部视窗 不加这一行图元里面会出现残影噪声
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //不设置大小会出现拖动释放后位置跳变，这个大小就是背景图片的实际大小
    QPixmap pm(":/img/cn.png");
    // qDebug() << "pm.rect()" << pm.rect(); //QRect(0,0 2220x1615)
    graphicsScene->setSceneRect(pm.rect());

    // 接收来之scene的item的信号
    connect (graphicsScene, &MainWindowNewScene::itemOperate, [ = ](Menu_iteamOperation::OperateType operate, Page_structure_utils::window_type list_type, QString id)
    {
        this->itemOperateSlot(operate, list_type, id);
    });

    // 控制参数编辑和删除设备图元能否使用
    connect (graphicsScene, &MainWindowNewScene::showParamSettingBtn, [&](bool tof)
    {
        ui->actionedit->setEnabled(tof);
        ui->actiondelete->setEnabled(tof);
    });

    // 控制工具栏中的拖动按钮
    connect(ui->graphicsView, &myGraphicsView::showTranslateBtn, [&](bool tof)
    {
        ui->actionDrag->setEnabled(tof);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initOpenGL()
{
//    openGL = new OpenGL();
//    openGL->setWindowTitle(tr("OpenGL"));
}

/**
 * @brief 加载项目工程文件，读取
 * @param pro_path 项目文件.dpsp路径
 * @return 是否读取成功
 */
bool MainWindow::loadProject(QString pro_path)
{
#if 0
    // 读取文本文件方式
    QFile pro(pro_path);
    if(pro.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&pro);
        // 设置编码格式 不然中文乱码
        in.setCodec("UTF-8");
        QString line = in.readLine();
        while (!line.isNull())
        {
//          qDebug() << line;
            line = in.readLine();
        }
        return true;
    }
    else
    {
        return false;
    }
#endif
    // 传入的是引用，相当于地址，读取完后会发生改变
    return Utils::readProjectXml(pro_path, info, comps);
}

void MainWindow::on_actionOpenGL_triggered()
{
    openGL->exec();
}

//读取设置
void MainWindow::readSettings()
{
    QSettings setting("DPSP_MainOpen", "Digit-pro");
    QPoint pos = setting.value("pos", QPoint(200, 200)).toPoint();
    QSize size = setting.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

//写入设置
void MainWindow::writeSettings()
{
    QSettings setting("DPSP_MainOpen", "Digit-pro");
    setting.setValue("pos", pos());
    setting.setValue("size", size());
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindow::closeEvent 关闭主窗口时的动作，保存应用设置
 * @param event 关闭事件
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    // TODO 先关闭其他子窗口  //shao:FIXME:这里要修改，和保存整体工程和雷达编辑窗口等相关性很强
//    if(ui->listWidget_rader->closeDragListWidget() && ui->listWidget_ecm->closeDragListWidget() && ui->listWidget_object->closeDragListWidget())
        if(1)//shao：FIXME：上面的函数删掉了。保存的方式也要改
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("退出程序"),
                                      "退出之前是否需要更新当前工程数据？",
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            saveStatus();
            event->accept();
        }
        else if (reply == QMessageBox::No)
        {
            qDebug() << "不保存数据->退出";
            event->accept();
        }
        else
        {
            qDebug() << "不退出";
            event->ignore();
        }
    }
    else
    {
        event->ignore();
    }
}

// This is an auto-generated comment for Antrn.
/**
 * @author Antrn
 * @date 2020-10-17 T 21:12:28
 * @copyright (c)
 * @description 保存当前装备列表中新建或者导入的项目
 */
bool MainWindow::saveStatus()
{
    QString path = QDir::currentPath() + "/project";
    // 上一级文件夹
    QString patt = path + "/" + w_title;
    // 根据用户的输入决定保存的工程的文件名
    QString pat = patt + "/" + w_title + ".dpsp";
    qDebug() << "退出主窗口！";
    //            qDebug() << "nameSet" << ui->listWidget_rader->nameSet;
    //            qDebug() << "保存之前看一下" << ui->listWidget_rader->getId_item();

    // 必须清空，不然一直都是一开始读取的原样
    comps.clear();

    for (auto i : Store::getWindow_list()[Page_structure_utils::radar].keys())
    {
        if(!comps.values().contains(i))
        {
            QString uuid = QUuid::createUuid().toString();
            comps.insert("radar" + uuid.mid(1, uuid.length() - 2), i);
        }
        else
        {
            qDebug() << "dpsp中的 <components> 中已存在" << i << "标签";
        }
    }
    for (auto i :  Store::getWindow_list()[Page_structure_utils::ecm].keys())
    {
        if(!comps.values().contains(i))
        {
            QString uuid = QUuid::createUuid().toString();
            comps.insert("ecm" + uuid.mid(1, uuid.length() - 2), i);
        }
        else
        {
            qDebug() << "dpsp中的 <components> 中已存在" << i << "标签";
        }
    }
    for (auto i :  Store::getWindow_list()[Page_structure_utils::object].keys())
    {
        if(!comps.values().contains(i))
        {
            QString uuid = QUuid::createUuid().toString();
            comps.insert("object" + uuid.mid(1, uuid.length() - 2), i);
        }
        else
        {
            qDebug() << "dpsp中的 <components> 中已存在" << i << "标签";
        }
    }
    qDebug() << "comps组件：" << comps;
    Utils::writeProjectXml(pat, w_title, info, comps);
    // TODO 保存设置
    writeSettings();
    qDebug() << "已保存数据->退出";
    this->isSave=true;
    return true;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindow::itemOperateSlot 接收scene发送的信号，进行下一步处理
 * @param operateType 操作方式
 * @param equipType 设备种类，对抗/雷达/目标
 * @param id 设备名称
 * @param newName 新名字
 */
void MainWindow::itemOperateSlot(Menu_iteamOperation::OperateType operateType, Page_structure_utils::window_type list_type, QString item_name)
{
    QString new_name;
    //shao：用来写独特操作的，有的现在还没啥用
    if(operateType == Menu_iteamOperation::del)
    {
        //shao：先询问是否确定
        if(QMessageBox::Ok != QMessageBox::question(this, "question",
                "删除此控件，场景中已经添加的组件"
                "也将被删除！是否还要继续？",
                QMessageBox::Ok, QMessageBox::Cancel))
        {
            return;
        }
        // weishi:删除雷达操作
        else
        {
            QString path = QDir::currentPath() + "/radar/" + item_name;
            if(path.isEmpty())
            {
                qDebug() << "路径不存在";
                return;
            }
            QDir dir(path);
            if(!dir.exists())
            {
                qDebug() << "该文件不存在";
                return;
            }
            dir.removeRecursively();
        }
        //shao:没保存就不删
        if(!Store::delet_window(list_type, item_name)){
                return;
        }
        this->isSave=false;
    }
    else if(operateType == Menu_iteamOperation::property)
    {
        //shao：还没有属性相关操作
        qDebug() << "[TO BE FIXED]设备属性窗口";
    }
    else if(operateType == Menu_iteamOperation::edit)
    {
        if(!Store::getWindow_list()[list_type].contains(item_name))//shao：为了解决设计缺陷引用入的一个bug。（先关闭窗口以后，store中的页面类就被删了，这个时候mainwindow页面的list里面还有这个页面 对应的item的，这时点击编辑，就会在stoer中找不到它，所以需要先导入）
            Store::open_window(list_type,item_name+"/");
        //shao：在新建(导入)item的时候已经新建好了，现在取出来
        if(list_type==Page_structure_utils::radar){
             MainWindow_Radar *window =  dynamic_cast <MainWindow_Radar*>(Store::getWindow_list()[list_type].value(item_name));
             window->setAttribute(Qt::WA_DeleteOnClose);
             emit set_window( Left_utils::development_board, Left_utils::switch_update,window);
        }
        else if (list_type==Page_structure_utils::ecm) {
            MainWindow_ECM *window =  dynamic_cast <MainWindow_ECM*>(Store::getWindow_list()[list_type].value(item_name));
            window->setAttribute(Qt::WA_DeleteOnClose);
            emit set_window( Left_utils::development_board, Left_utils::switch_update,window);
        }
        else if (list_type==Page_structure_utils::object) {
            MainWindow_object *window =  dynamic_cast <MainWindow_object*>(Store::getWindow_list()[list_type].value(item_name));
            window->setAttribute(Qt::WA_DeleteOnClose);
            emit set_window( Left_utils::development_board, Left_utils::switch_update,window);
        }
    }
    else if(operateType == Menu_iteamOperation::rename)
    {
        new_name=Store::edit_window_name(list_type,item_name);
        if(new_name.compare(item_name)==0){
            return;
        }
        this->isSave=false;
        emit this->set_window(Left_utils::development_board,Left_utils::only_update,Store::getWindow_list()[list_type].value(new_name));
    }
    else if(operateType == Menu_iteamOperation::ppi)
    {}
    else if(operateType == Menu_iteamOperation::ashow)
    {}
    this->graphicsScene->itemOperateSlot(operateType, list_type, item_name,new_name);
    ui->listWidget_rader->itemOperateSlot(operateType, list_type, item_name,new_name);
    ui->listWidget_ecm->itemOperateSlot(operateType, list_type, item_name,new_name);
    ui->listWidget_object->itemOperateSlot(operateType, list_type, item_name,new_name);
}

void MainWindow::on_actionexit_triggered()
{
    // 软件退出
//    qApp->closeAllWindows();
    close();
}

void MainWindow::on_actionCurve_triggered()
{
//    RealTimeCurve *rtc = new RealTimeCurve;
//    rtc->show();
}

void MainWindow::on_actionToolsBar_triggered()
{
    //显示工具栏
    if(ui->mainToolBar->isHidden())
    {
        ui->mainToolBar->show();
    }
    else
    {
        ui->mainToolBar->hide();
    }
}

void MainWindow::on_actionEquip_triggered()
{
    //显示装备栏
    if(ui->dockWidget_left->isVisible())
    {
        ui->dockWidget_left->close();
    }
    else
    {
        ui->dockWidget_left->show();
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-10
 * @brief MainWindow::on_actiondataPlayBack_triggered 当出发数据回访时候，新开启一个窗口
 */
void MainWindow::on_actiondataPlayBack_triggered()
{
    DataPlayBackWidget *dw = new DataPlayBackWidget(w_title, nullptr);
    dw->show();
    QDesktopWidget *deskdop = QApplication::desktop();
    dw->move((deskdop->width() - 250) / 2, (deskdop->height() - 200) / 2); //高正中间应该是184
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief MainWindow::on_actionedit_triggered 用户选择某个已添加到场景中的设备后，才能点击设置参数，三种设置通用，根据设备的不同动态显示页面
 */
void MainWindow::on_actionedit_triggered()
{
    SimupParamSetting *sps = new SimupParamSetting();
    // TODO 获取当前用户选择的是什么设备，决定页面显示的内容
    QList<QGraphicsItem*> items = graphicsScene->selectedItems();
    RadarItem *ri;
    ECMItem *ei;
    if (!items.isEmpty())
    {
        if((ri = qgraphicsitem_cast<RadarItem*>(items.at(0))))
            sps->setWindowTitle(ri->getRadar_id());
        if((ei = qgraphicsitem_cast<ECMItem*>(items.at(0))))
            sps->setWindowTitle(ei->getEcm_id());
        else
        {
            sps->setWindowTitle("目标");
        }
    }
    sps->setWindowModality(Qt::NonModal);//非模态
    sps->show();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief MainWindow::on_actionsaveproject_triggered 保存仿真场景和参数设置内容
 */
void MainWindow::on_actionsaveproject_triggered()
{
    // TODO 获取用户设置的针对场景中的所有组件的设置和场景中的地理信息系统的具体信息，比如速度，方位坐标、经纬度等
    saveStatus();
    Utils::alert(QApplication::desktop()->rect(), "已经保存！");
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief MainWindow::on_actionReset_triggered 清除场景中所有的items，调用中心函数，将视图定位到左上角
 */
void MainWindow::on_actionReset_triggered()
{
    // 默认可以多选，按住Ctrl+点选是多选
//    QList<QGraphicsItem*> si = graphicsScene->items();
//    for(auto i: si){
//        qDebug() << i;
//        graphicsScene->removeItem(i);
//        delete i;  //释放内存
//    }
//    si.clear();
    graphicsScene->clear();
    on_actionCenter_triggered();
    // 清除开始图元记录
    graphicsScene->setlineBeginItem(nullptr);
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief MainWindow::on_actionCenter_triggered 复位到左上角，命名为center有点不准确，但也可以默认定位到中心
 */
void MainWindow::on_actionCenter_triggered()
{
    // 将 view 的中心点定位到左上角
    ui->graphicsView->centerOn(QPoint(0, 0));
}

void MainWindow::on_actionZoomin_triggered()
{
    ui->graphicsView->zoomIn();
}

void MainWindow::on_actionZoomout_triggered()
{
    ui->graphicsView->zoomOut();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief MainWindow::on_actiondelete_triggered 删除图元，实际上是可以多选的
 */
void MainWindow::on_actiondelete_triggered()
{
    // 默认可以多选，按住Ctrl+点选是多选
    QList<QGraphicsItem*> si = graphicsScene->selectedItems();
    int len = si.size();
    RadarItem *ri;
    ArrowItem *ai;
    // TODO 电子对抗 目标设备删除时候的判断
    for(int i = 0; i < len; i++)
    {
//        qDebug() << si[i] << si[i]->scene();
        // 不能直接用这个，报警告 QGraphicsScene::removeItem: item 0x280f1e40's scene (0x0) is different from this scene (0x28051540)
//        graphicsScene->removeItem(si[i]);
        if((ri = qgraphicsitem_cast<RadarItem*>(si[i])))
        {
            ri->destroyed();
        }
        if((ai = qgraphicsitem_cast<ArrowItem*>(si[i])))
        {
            ai->destroyed();
        }
        //
        //if...
        //if...
        delete si[i];  //释放内存
    }
    si.clear();
    // 清除开始图元记录指针，防止出现已经选中删除一个图元，再点另一个图元导致之前记录的开始图元指针异常
    graphicsScene->setlineBeginItem(nullptr);
}

QString MainWindow::new_a_prohect(QString pro_name){
        // 工程的配置文件信息
        QMap<QString, QString> infos;
        QString path = QDir::currentPath()+"/project"+"/"+pro_name;
        // 根据用户的输入决定保存的工程的文件名
        QString pat = path+"/"+pro_name+".dpsp";
        // 没有的话，先创建
        Utils::openDirOrCreate(path+"/data");
        // 打开文件，没有的话会自己创建
        QFile file(pat);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
            qDebug() << "打开文件出现错误";
            file.close();
         }else{
            // 获得所有组件的名字列表
            // QList<QString>item_names =  ui->listWidget_rader->getId_item().keys();
            // 生成project的id，name属性
            infos.insert("name", pro_name);
            infos.insert("id", QUuid::createUuid().toString());
            qDebug() << "工程文件"+pro_name+".dpsp保存" << ((Utils::writeProjectXml(pat, pro_name,infos,comps)==true)?"成功":"失败") << "路径为："+pat;
            // 添加到项目列表
            QString right_path = pat.right(pat.length() - QDir::currentPath().length());
            Utils::addProject2Pl(pro_name, right_path);
         }
         return pat;
}
