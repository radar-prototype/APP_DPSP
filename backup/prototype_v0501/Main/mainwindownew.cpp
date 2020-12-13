#include "mainwindownew.h"
#include "ui_mainwindownew.h"
#include "draglistwidget.h"
#include <QDesktopWidget>
#include <QtDebug>
#include <QListWidgetItem>
#include <QMessageBox>
#include <utils.h>
#include <QFileDialog>
#include <QInputDialog>

//QList<MainWindow_Radar *> MainWindowNew::main_radar_list;
/**
* @projectName   prototype_v0719
* @brief         新建工程时的主窗口。
* @author        Antrn
* @date          2019-08-12
*/

MainWindowNew::MainWindowNew(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowNew)
{
    ui->setupUi(this);
    // 设置对话框icon
    this->setWindowIcon(QIcon(":/img/fangzhen.png"));
    this->setAttribute(Qt::WA_DeleteOnClose);

    ui->listWidget_ecm->listItem_add(ui->listWidget_ecm->getEcmType());
    ui->listWidget_object->listItem_add(ui->listWidget_object->getTarType());
    ui->listWidget_rader->listItem_add(ui->listWidget_rader->getRadarType());

    graphicsScene=new MainWindowNewScene();
    // xy坐标标签
    label_xy=new QLabel(this);
    // 状态栏左边加入xy坐标
    ui->statusBar->addWidget(label_xy);
    // 将主窗口中的场景设置为自定义的场景
    ui->graphicsView->setScene(graphicsScene);
    // 设置鼠标跟踪开启
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setAcceptDrops(true);
    // 默认定位到雷达
    ui->toolBox_left->setCurrentIndex(0);
    //不设置大小会出现拖动释放后位置跳变
//    graphicsScene->setSceneRect(QRectF(0, 0, 5000, 5000));
    QPixmap pm(":/img/cn.png");
    // qDebug() << "pm.rect()" << pm.rect(); //QRect(0,0 2220x1615)
    graphicsScene->setSceneRect(pm.rect());

    connect(graphicsScene,SIGNAL(signal_xy(double,double)),this,SLOT(xy_show(double,double)));
//    this->showMaximized();
    this->setWindowTitle(tr("新建工程"));
    //可以设置在视图上使用鼠标拖出橡皮筋框选择图形项
    this->ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    // 接收来之左边item的信号
    connect(this->ui->listWidget_rader,&DragListWidget::itemOperate,this,&MainWindowNew::itemOperateSlot);
    connect(this->ui->listWidget_ecm,&DragListWidget::itemOperate,this,&MainWindowNew::itemOperateSlot);
    connect(this->ui->listWidget_object,&DragListWidget::itemOperate,this,&MainWindowNew::itemOperateSlot);
    //切换窗口
    connect(this->ui->listWidget_rader, &DragListWidget::set_window, this, &MainWindowNew::set_window);
    connect(this->ui->listWidget_ecm, &DragListWidget::set_window, this, &MainWindowNew::set_window);
    connect(this->ui->listWidget_object, &DragListWidget::set_window, this, &MainWindowNew::set_window);
    // 接收来之scene的item的信号   shao：修改了graphicsScene信号会报错注释了
//    connect (graphicsScene, &MainWindowNewScene::itemOperate,[=](Menu_iteamOperation::OperateType operate, QString etype, QString id){
//        this->itemOperateSlot(operate, etype, id);
//    });
    // 默认属性窗口隐藏
    ui->dockWidget_property->hide();
}

MainWindowNew::~MainWindowNew()
{
    delete ui;
}


// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindowNew::saveListContent2pro 单独抽取出来的保存三种组件设备的列表的函数
 */
bool MainWindowNew::saveListContent2pro()
{
    bool ok;
    QString pro_name = QInputDialog::getText(this, tr("保存工程"),tr("请输入保存的工程名"), QLineEdit::Normal, nullptr, &ok);
    if (ok && !pro_name.isEmpty())
    {
        QString path = QDir::currentPath()+"/project";
        if (!path.isEmpty()){
            // 上一级文件夹
            QString patt = path+"/"+pro_name;
            // 根据用户的输入决定保存的工程的文件名
            QString pat = patt+"/"+pro_name+".dpsp";
            // 没有的话，先创建
            Utils::openDirOrCreate(patt+"/data");
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
                // 雷达
                for (auto n : ui->listWidget_rader->getId_item().keys()){
                    QString rand_id = QUuid::createUuid().toString();
                    comps.insert("radar"+rand_id.mid(1,rand_id.length()-2),n);
                }
                // 对抗
                for (auto n : ui->listWidget_ecm->getId_item().keys()){
                    QString rand_id = QUuid::createUuid().toString();
                    comps.insert("ecm"+rand_id.mid(1,rand_id.length()-2),n);
                }
                // 目标
                for (auto n : ui->listWidget_object->getId_item().keys()){
                    QString rand_id = QUuid::createUuid().toString();
                    comps.insert("object"+rand_id.mid(1,rand_id.length()-2),n);
                }
                qDebug() << "工程文件"+pro_name+".dpsp保存" << ((Utils::writeProjectXml(pat, pro_name,infos,comps)==true)?"成功":"失败") << "路径为："+pat;
                // 添加到项目列表
                QString right_path = pat.right(pat.length() - QDir::currentPath().length());
                Utils::addProject2Pl(pro_name, right_path);
                return true;
            }
        }
    }else{
        qDebug() << "输入为空！";
        Utils::alert(QApplication::desktop()->screen()->rect(), "输入为空！");
        return false;
    }
    return false;
}

/**
 * @brief 窗口关闭事件
 * @param event
 */
void MainWindowNew::closeEvent(QCloseEvent *event)
{
#if 0
    for(MainWindow_Radar *m : main_radar_list){
        m->close();
    }
    main_radar_list.clear();
# endif


    // 点击菜单栏中的exit也会触发closeEvent函数，重复了，所以把主要逻辑移到这里面来
    // 这是一个小坑，已填
    if(ui->listWidget_rader->closeDragListWidget() && ui->listWidget_ecm->closeDragListWidget() && ui->listWidget_object->closeDragListWidget()){
        int ret1 = QMessageBox::question(this, tr("确认退出工程"), tr("退出前保存文件?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
        if(ret1 == QMessageBox::Yes){
            // 成功保存
            if(saveListContent2pro()){
                qApp->closeAllWindows();
            }else
                event->ignore();
        }else if(ret1 == QMessageBox::No){
            qDebug() << "不保存!";
            event->accept();
        }else {
            qDebug() << "拒绝关闭!";
            event->ignore();
        }
    }else
        event->ignore();
}

//坐标跟踪
void MainWindowNew::xy_show(double x,double y)
{
    QString str=QString("x=%1,y=%2").arg(QString::number(x)).arg(QString::number(y));
    label_xy->setText(str);
}
//关于我们
void MainWindowNew::on_actionabout_DPSP_triggered()
{
    QMessageBox::about(this, tr("关于此软件 DPSP"),
                             tr("[EXAMPLE:]> The <b>DPSP(Digital-Prototype-Simulation-Platform)</b>"
                                " is an engineering level simulation tool for radar and electronic countermeasures equipment.."));
}

/**
 * @brief MainWindowNew::on_actionexit_triggered 退出的时候提示是否保存工程文件
 * 并将其写入到projectlist.pl文件中
 */
void MainWindowNew::on_actionexit_triggered()
{
    // 和closeEvent功能重复，所以整体迁移到closeEvent方法中
#if 0
    int ret1 = QMessageBox::question(this, tr("确认退出工程"), tr("退出前保存文件?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    if(ret1 == QMessageBox::Yes){
        bool ok;
        QString pro_name = QInputDialog::getText(this, tr("保存工程"),tr("请输入保存的工程名"), QLineEdit::Normal, nullptr, &ok);
        if (ok && !pro_name.isEmpty())
        {
            QString path = QDir::currentPath()+"/project";
            if (!path.isEmpty())
            {
                // 上一级文件夹
                QString patt = path+"/"+pro_name;
                // 根据用户的输入决定保存的工程的文件名
                QString pat = patt+"/"+pro_name+".dpsp";
                // 没有的话，先创建
                Utils::openDirOrCreate(patt+"/data");
                // 打开文件，没有的话会自己创建
                QFile file(pat);
                if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
                    qDebug() << "打开文件出现错误";
                    file.close();
                }else{
                    // 获得所有组件的名字列表
//                    QList<QString>item_names =  ui->listWidget_rader->getId_item().keys();
                    // 生成project的id，name属性
                    infos.insert("name", pro_name);
                    infos.insert("id", QUuid::createUuid().toString());
                    // 雷达
                    for (auto n : ui->listWidget_rader->getId_item().keys()){
                        QString rand_id = QUuid::createUuid().toString();
                        comps.insert("radar"+rand_id.mid(1,rand_id.length()-2),n);
                    }
                    // 对抗
                    for (auto n : ui->listWidget_ecm->getId_item().keys()){
                        QString rand_id = QUuid::createUuid().toString();
                        comps.insert("ecm"+rand_id.mid(1,rand_id.length()-2),n);
                    }
//                    QString prename;
//                    if(n.contains("雷达")){
//                        prename = "radar";
//                    }else if(n.contains("对抗")){
//                        prename = "ecm";
//                    }else if(n.contains("目标")){
//                        prename = "object";
//                    }
                    // 目标
                    for (auto n : ui->listWidget_object->getId_item().keys()){
                        QString rand_id = QUuid::createUuid().toString();
                        comps.insert("object"+rand_id.mid(1,rand_id.length()-2),n);
                    }
                    qDebug() << "工程文件"+pro_name+".dpsp保存" << ((Utils::writeProjectXml(pat, pro_name,infos,comps)==true)?"成功":"失败") << "路径为："+pat;
                    // 添加到项目列表
                    QString right_path = pat.right(pat.length() - QDir::currentPath().length());
                    Utils::addProject2Pl(pro_name, right_path);
                    qApp->closeAllWindows();
                }
            }
        }else{
            qDebug() << "输入为空！";
            Utils::alert(QApplication::desktop()->screen()->rect(), "输入为空！");
            return;
        }

//        QString spath = QFileDialog::getExistingDirectory(
//                this,
//                "选择文件夹",
//                path,
//                QFileDialog::ShowDirsOnly);

    }else if(ret1 == QMessageBox::No){
        qDebug() << "不保存";
        qApp->closeAllWindows();
    }else {
        qDebug() << "拒绝关闭!!!";
        return;
    }
#endif
    qApp->closeAllWindows();
}

//左边窗口显示
void MainWindowNew::on_actio_leftDock_triggered()
{
    if(ui->dockWidget_left->isHidden()){
        ui->dockWidget_left->show();
    }else{
        ui->dockWidget_left->hide();
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindowNew::itemOperateSlot 下发操作指令，和mainWindow一模一样不正日
 * @param operateType 操作类型
 * @param equipType 设备种类：radar/ecm/object
 * @param id 设备名称
 * @param newName 新名字
 */
void MainWindowNew::itemOperateSlot(Menu_iteamOperation::OperateType operateType, QString equipType, QString id, QString newName){
    qDebug()<<"mainWindowsNew::"<<operateType<<","<<id<<"new name:"<<newName;

    if(operateType==Menu_iteamOperation::del){
//        this->graphicsScene->itemOperateSlot(Menu_iteamOperation::del, equipType, id);
        if(equipType=="radar"){
            ui->listWidget_rader->itemOperateSlot(Menu_iteamOperation::del,id);
        }else if (equipType=="ecm")
            ui->listWidget_ecm->itemOperateSlot(Menu_iteamOperation::del,id);
        else if (equipType=="object")
            ui->listWidget_object->itemOperateSlot(Menu_iteamOperation::del,id);
    }else if(operateType==Menu_iteamOperation::property){
        //弹出属性窗
        ui->dockWidget_property->show();
        ui->actionProperty->setChecked(true);
        //属性窗口内容显示
        propertyContent(id);
    }else if(operateType==Menu_iteamOperation::edit){
        // 新建或者提升组件窗口（从left的item来，他在就在，他死就死）
        qDebug()<<"左边列表的建模/编辑信号来了";
        if(equipType=="radar"){
            ui->listWidget_rader->itemOperateSlot(Menu_iteamOperation::edit,id);
        }else if (equipType=="ecm")
            ui->listWidget_ecm->itemOperateSlot(Menu_iteamOperation::edit,id);
        else if (equipType=="object")
            ui->listWidget_object->itemOperateSlot(Menu_iteamOperation::edit,id);
    }else if(operateType==Menu_iteamOperation::rename){
//        this->graphicsScene->itemOperateSlot(Menu_iteamOperation::rename,id,newName);
        if(equipType=="radar"){
            ui->listWidget_rader->itemOperateSlot(Menu_iteamOperation::rename,id,newName);
        }else if (equipType=="ecm")
            ui->listWidget_ecm->itemOperateSlot(Menu_iteamOperation::rename,id,newName);
        else if (equipType=="object")
            ui->listWidget_object->itemOperateSlot(Menu_iteamOperation::rename,id,newName);
    }else if(operateType==Menu_iteamOperation::ppi){
//        this->graphicsScene->itemOperateSlot(Menu_iteamOperation::ppi,id,newName);
        if(equipType=="radar"){
            ui->listWidget_rader->itemOperateSlot(Menu_iteamOperation::ppi,id,newName);
        }else if (equipType=="ecm")
            ui->listWidget_ecm->itemOperateSlot(Menu_iteamOperation::ppi,id,newName);
        else if (equipType=="object")
            ui->listWidget_object->itemOperateSlot(Menu_iteamOperation::ppi,id,newName);
    }else if(operateType==Menu_iteamOperation::ashow){
//        this->graphicsScene->itemOperateSlot(Menu_iteamOperation::ashow,id,newName);
        if(equipType=="radar"){
            ui->listWidget_rader->itemOperateSlot(Menu_iteamOperation::ashow,id,newName);
        }else if (equipType=="ecm")
            ui->listWidget_ecm->itemOperateSlot(Menu_iteamOperation::ashow,id,newName);
        else if (equipType=="object")
            ui->listWidget_object->itemOperateSlot(Menu_iteamOperation::ashow,id,newName);
    }
 }

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindowNew::propertyContent 属性窗口内容显示
 * @param id 设备名称
 */
void MainWindowNew::propertyContent(QString id){
    // 读取XMl写入窗口
    qDebug() << "[TO BE FIXED]组件name:" << id << "留下了";
    // TODO 属性展示
}

 /**
  * @brief MainWindowNew::on_actionProperty_triggered 显示属性dock面板
  * @param checked
  */
 void MainWindowNew::on_actionProperty_triggered(bool checked)
{
    if(checked){
        ui->dockWidget_property->show();
    }else{
        ui->dockWidget_property->hide();
    }
}

 /**
  * @brief 保存工程.dpsp
  */
void MainWindowNew::on_actionsaveFile_triggered()
{
    // 保存工程包含几个radar，几个ecm，几个object，各自的属性以及位置等信息到.dpsp文件中
    bool tof = saveListContent2pro();
    qDebug() << "保存成功?" << tof;
}

/**
 * @brief 用户想要在此窗口中另外新建一个工程
 */
void MainWindowNew::on_actionnewFile_triggered()
{
    // TODO 新建.dpsp工程文件，弹出另外一个窗口
    MainWindowNew *another = new MainWindowNew(nullptr);
    another->showNormal();
}
