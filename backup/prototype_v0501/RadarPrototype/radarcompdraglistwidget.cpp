#include "radarcompdraglistwidget.h"
#include <QDrag>
#include <QDebug>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QSizePolicy>
#include <QGridLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <qfiledialog.h>
#include <algorithmcomp.h>
#include <parameditradardialog.h>
#include "mainwindow_radar.h"
#include "utils.h"
#include <QApplication>
#include <QDesktopWidget>
#include <algocodeedit.h>
#include <codewindow.h>
#include <QDebug>
// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介: 雷达组件列表
* @author        Antrn
* @date          2019-09-27
*/
RadarCompDraglistWidget::RadarCompDraglistWidget(QWidget *parent) : QListWidget(parent)
{
    // 设置允许接收拖拽
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setSpacing(5);

    // icon图标显示
    setViewMode(QListView::ListMode);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //垂直滚动条
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //水平滚动条
    // 设置ICON大小
    setIconSize(QSize(50, 50));

    this->setDropIndicatorShown(true);
    // 设置拖放模式为移动项目，否则为复制项目
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setAttribute(Qt::WA_PendingMoveEvent);

    // 加入新建雷达按钮项
    this->addItem(tr("自定义组件"));
    addCompButton = this->item(0);
    addCompButton->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    addCompButton->setTextColor(QColor(255, 0, 0));
    addCompButton->setFlags(Qt::NoItemFlags);
    addCompButton->setIcon(QIcon(":/img/newradar.png"));
    addCompButton->setToolTip(tr("点击增加自定义组件"));
    connect(this, &QListWidget::itemDoubleClicked, this, &RadarCompDraglistWidget::onCurrentDoubleClicked);
    connect(this, &QListWidget::itemChanged, this, &RadarCompDraglistWidget::onCurrentTextChanged);
}

void RadarCompDraglistWidget::addDragItem(QListWidgetItem*item)
{
    this->addItem(item);
    this->setVisible(true);
}
// This is an auto-generated comment.
/**
* @projectName   prototype_v0906
* @brief         简介 新建组件 新建了弹窗 ParamEditRadarDialog
* @author        Antrn
* @date          2019-10-05
*/
void RadarCompDraglistWidget::createNewComp()
{
    AlgorithmComp ac;
    // 新建
    ParamEditRadarDialog *dlg = new ParamEditRadarDialog(ac, this);
    // 先把要展示的消息传递给draglistWidget，再传给mainwindow_radar
    connect(dlg, &ParamEditRadarDialog::showMessage, this, &RadarCompDraglistWidget::sendMessage);
    connect(dlg, &ParamEditRadarDialog::sendIconName, this, &RadarCompDraglistWidget::send_icon_name);

    if(dlg->exec() == QDialog::Accepted)
    {
        qDebug() << "确定新建";
        algorithms.insert(dlg->ac.getInfo()["ID"], dlg->ac);
        qDebug() << "刚增加的id:" << dlg->ac.getInfo()["ID"];
        qDebug() << "algorithms： " << algorithms.keys() << "; 大小： " << algorithms.size();
        Utils::writeAlgorithmComp2Xml(dlg->ac);
        emit add_one_Comp(dlg->ac);
        emit toRefreshCompList(); //刷新列表
        qDebug() << "-------[重要draglist]---------" << dlg->ac.getInfo()["Name"];
        Utils::generateIcon(dlg->ac.getInfo()["Name"], "");
        Utils::alert(QApplication::desktop()->screen()->rect(), "添加组件成功!");
    }
    else
    {
        // reject
        qDebug() << "取消新建组件";
    }
}

/**
 * @brief 给雷达组件重命名
 * @param item 当前改变的是哪个组件
 */
void RadarCompDraglistWidget::onCurrentTextChanged(QListWidgetItem *item)
{
    // 不能是添加按钮
    if(item != addCompButton)
    {
        // 原来的名字
        QString lastName = item->text();
        qDebug() << "item名字变为： " << lastName << "； item内容有变化";
        if(nameList.contains(lastName))
        {
            qDebug() << "与现有的文件名: " << lastName << "重复!";
            Utils::alert(QApplication::desktop()->screen()->rect(), "已有重复名称存在，请重试");
        }
        else
        {
            // 删除原来的名字
            nameList.removeOne(oldName);
            qDebug() << oldName << "→" << lastName;
            if(Utils::modifyFileName(oldName, lastName))
            {
                AlgorithmComp ac;
                ac = algorithms.value(item->data(Qt::UserRole + 2).toString());
                ac.setFileName(lastName);
                // 新建info，只为了修改Name，真的sb
                QMap<QString, QString> newm;
                newm.insert("ID", ac.getInfo()["ID"]);
                newm.insert("Path", ac.getInfo()["Path"]);
                QDateTime *dt = new QDateTime;
                QString dtime = dt->currentDateTime().toString();
                newm.insert("Time", dtime);
                newm.insert("Name", lastName);
                ac.setInfo(newm);
                // 5.9.8
                //                qDebug() << "该算法的信息: " << ac.getInfo().toStdMap();
                Utils::writeAlgorithmComp2Xml(ac);
                // 刷新列表
                emit toRefreshCompList();
                // 生成新名字的icon
                emit send_icon_name(lastName);

                qDebug() << "重命名成功!";
            }
            else
            {
                qDebug() << "重命名失败";
            }
        }
    }
}

/**
 * 双击改名字
 * @brief RadarCompDraglistWidget::onCurrentDoubleClicked
 * @param item
 */
void RadarCompDraglistWidget::onCurrentDoubleClicked(QListWidgetItem *item)
{
    QString preName = item->text();
    if(!nameList.contains(preName))
    {
        qDebug() << "原名字: " << preName << "不存在，出现错误";
    }
    else
    {
        oldName = preName;
        qDebug() << "原名字： " << preName;
    }
}

/**
 * @brief 在左边组件库列表删除组件时候的槽函数
 */
void RadarCompDraglistWidget::deleteItemSlot()
{
    QListWidgetItem * item = currentItem();
    if( item == nullptr )
        return;
    // FIXME 这里也是用字符串匹配的
    if(item->text().compare("INPUT") == 0 || item->text().compare("OUTPUT") == 0)
    {
        qDebug() << "不允许删除输入输出组件!";
        QMessageBox *mb = new QMessageBox(nullptr);
        mb->setWindowTitle("警告");
        mb->setText("抱歉！您不能删除内置：‘" + item->text() + "’组件！");
        mb->show();
        return;
    }
    int ch = QMessageBox::warning(this, "提醒",
                                  "您确定要删除组件库中的此组件吗?\n若已有场景中使用了该组件，您的删除会造成致命错误。",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    if ( ch != QMessageBox::Yes )
        return;

    QString na = item->text();
    QString id = item->data(Qt::UserRole + 2).toString();
    qDebug() << "组件库-删除组件: " << na;
    takeItem(row(item));
    // nameList名字删除
    nameList.removeOne(na);
    // 这里面也要删
    algorithms.take(id);
    qDebug() << "组件库列表的algorithms： " << algorithms.keys() << "; 大小： " << algorithms.size();
    // 对应文件也要删除
    QList<QString> codes =
    {
        QDir::currentPath() + "/algoXml/" + na.toLower() + ".xml",
        QDir::currentPath() + "/codes/" + na.toLower() + ".c",
        QDir::currentPath() + "/codes/" + na.toLower() + ".h",
        QDir::currentPath() + "/codes/" + na.toLower() + ".txt"
    };
    for(int i = 0; i < 4; i++)
    {
        QFile file(codes.at(i));
        file.remove();
        file.close();
    }
    //    QFile file(QDir::currentPath()+"/algoXml/"+na+".xml");
    //    file.remove();
    //    file.close();
    //    QFile filec(QDir::currentPath()+"/codes/"+na+".c");
    //    filec.remove();
    //    filec.close();
    //    QFile fileh(QDir::currentPath()+"/codes/"+na+".h");
    //    fileh.remove();
    //    fileh.close();
    //    QFile filet(QDir::currentPath()+"/codes/"+na+".txt");
    //    filet.remove();
    //    filet.close();
    delete item;
    // Antrn: 这里注意一下，不知道会不会有bug
    emit setComp_typeandMode("", RadarScene::MoveItem);
}

/**
 * 修改组件的槽函数，用于修改参数
 * @brief RadarCompDraglistWidget::editItemParamSlot
 */
void RadarCompDraglistWidget::editItemParamSlot()
{
    QListWidgetItem * item = currentItem();
    if( item == nullptr )
        return;
    qDebug() << "要编辑的组件名字为: " << item->text();

    AlgorithmComp ac;
    ac = algorithms.value(item->data(Qt::UserRole + 2).toString());
    ParamEditRadarDialog *dlg = new ParamEditRadarDialog(ac, this, 1);
    if(dlg->exec() == QDialog::Accepted)
    {
        qDebug() << "确定编辑";
        Utils::writeAlgorithmComp2Xml(dlg->ac);
        emit add_one_Comp(dlg->ac);
        emit toRefreshCompList(); //刷新列表
    }
    else
    {
        qDebug() << "取消编辑";
    }
}

/**
 * 算法组件库中代码编辑的槽函数，弹出窗口
 * @brief RadarCompDraglistWidget::codeItemEditSlot
 */
void RadarCompDraglistWidget::codeItemEditSlot()
{
    CodeWindow *cw = new CodeWindow(this, currentItem());
    cw->show();
#if 0
    // 这里没有进行组件之间的绑定，下面的功能转移到上面的codewindow了
    AlgoCodeEdit *child = new AlgoCodeEdit;
    // 设置tab距离
    child->setTabStopWidth(4);
    child->setMinimumSize(800, 600);
    QListWidgetItem * item = currentItem();
    if( item == nullptr )
        return;
    QString na = item->text();
    QString id = item->data(Qt::UserRole + 2).toString();
    qDebug() << "编辑组件代码: " << na;
    bool tof = child->loadFile(QDir::currentPath() + "/codes/" + na + ".c");
    if(tof)
    {
        child->showNormal();
        child->move((QApplication::desktop()->width() - child->width()) / 2, (QApplication::desktop()->height() - child->height()) / 2);
    }
    connect(child, SIGNAL(copyAvailable(bool)), ui->actionCut, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), ui->actionCopy, SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));
#endif
}

void RadarCompDraglistWidget::createItemParamSlot()
{
    createNewComp();
}

// This is an auto-generated comment.
/**
* @projectName   prototype_v0906
* @brief         简介 算法组件item右键菜单
* @author        Antrn
* @date          2019-10-05
*/
void RadarCompDraglistWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidgetItem *currItem = itemAt(event->pos());
    QMenu *popMenu = new QMenu(this);
    // 暂时在这里添加几个右键菜单了，包括删除、代码编辑和编辑属性
    QAction *addAct = nullptr, *deleteAct = nullptr, *editAct = nullptr, *codeEditAction;
    if(currItem != nullptr && currItem != addCompButton)
    {
        qDebug() << "当前组件的text： " << currItem->text();
        deleteAct = new QAction(tr("删除组件"), this);
        editAct = new QAction(tr("修改参数"), this);
        codeEditAction = new QAction(tr("修改代码"), this);
        deleteAct->setIcon(QIcon(":/img/delete.png"));
        editAct->setIcon(QIcon(":/img/editComp.png"));
        codeEditAction->setIcon(QIcon(":/img/code.png"));
        popMenu->addAction(deleteAct);
        popMenu->addAction(editAct);
        // Antrn: NOTE 这里暂时设置为不可点，因为会有空指针异常
        codeEditAction->setEnabled(false);
        popMenu->addAction(codeEditAction);
        connect(deleteAct, &QAction::triggered, this, &RadarCompDraglistWidget::deleteItemSlot);
        connect(editAct, &QAction::triggered, this, &RadarCompDraglistWidget::editItemParamSlot);
        connect(codeEditAction, &QAction::triggered, this, &RadarCompDraglistWidget::codeItemEditSlot);
    }
    else
    {
        addAct = new QAction(tr("添加组件"), this);
        addAct->setIcon(QIcon(":/img/addComp.png"));
        popMenu->addAction(addAct);
        connect(addAct, &QAction::triggered, this, &RadarCompDraglistWidget::createItemParamSlot);
        qDebug() << "此位置没有组件，或者是第一个自定义按钮";
    }
    popMenu->exec(QCursor::pos());
    delete popMenu;
    delete deleteAct;
    delete editAct;
    delete addAct;
}

void RadarCompDraglistWidget::dealCursor(QString str)
{
    if(str == "close")
    {
        setCursor(Qt::ClosedHandCursor);
    }
}

/**
 * @brief 开始拖动时
 */
void RadarCompDraglistWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    // weishi:用户拖拽组件时鼠标样式改变
    //setCursor(Qt::ClosedHandCursor);
    qDebug()<<"startDrag函数被调用";
    //可以直接使用currentItem获得当前的组件
    QListWidgetItem *item = currentItem();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    //分别从用户角色中获取信息
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    QString str_name = qvariant_cast<QString>(item->data(Qt::UserRole + 1));

    // 将组件的id存进去了
    QString id = qvariant_cast<QString>(item->data(Qt::UserRole + 2));
    // 复制一份
    AlgorithmComp ap =  algorithms[id];
    emit addAlgo2Scene(ap);

    //设置dataStream
    dataStream << pixmap << str_name << id;
    //    qDebug() << "pixmap: " << pixmap;
    //    qDebug() << "picture str_name: " << str_name;

    QMimeData *mimeData = new QMimeData;
    //设置自定义mime数据
    mimeData->setData(RadarCompDraglistWidget::puzzleMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //鼠标始终保持在元件的中心
    drag->setHotSpot(QPoint(25, 25));
    //缩放图片
    drag->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));

    if (drag->exec(Qt::MoveAction) == Qt::CopyAction)
    {
        qDebug() << "复制动作";
    }
}

//重写鼠标点击操作.
void RadarCompDraglistWidget::mousePressEvent(QMouseEvent *event)
{
    //确保左键拖拽.
    if (event->button() == Qt::LeftButton)
    {
        // qDebug() << "item被点击";
        // weishi:用户拖拽组件时鼠标样式改变
        //viewport()->setCursor(Qt::ClosedHandCursor);

        // 先保存拖拽的起点.
        m_dragPoint = event->pos();
        // 保留被拖拽的项.
        m_dragItem = this->itemAt(m_dragPoint);
        // 如果点击项是新建项则新建组件
        if(m_dragItem == addCompButton)
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle("添加组件");
            msgBox.setText("添加雷达组件");
            msgBox.setInformativeText("您想要创建一个新的雷达组件，还是导入一个已经有的雷达组件？");
            QPushButton *newButton = msgBox.addButton(tr("新建"), QMessageBox::ActionRole);
            msgBox.addButton(tr("导入"), QMessageBox::ActionRole);
            msgBox.addButton(tr("取消"), QMessageBox::RejectRole);
            msgBox.setDefaultButton(newButton);
//            msgBox.move ((QApplication::desktop()->width() - msgBox.width()) / 2, (QApplication::desktop()->height() - msgBox.height()) / 2);
            int button_index = msgBox.exec();
            switch (button_index)
            {
            case 2:
                qDebug() << "不新建组件也不导入，关闭弹窗";
                break;
            // 创建
            case 0:
            {
                createNewComp();
#if 0
                int flag = 0;
                QString Compname = "";
                while(Compname.isEmpty() || Compname.isNull())
                {
                    // 弹窗填写参数
                    QInputDialog dlg;
                    dlg.setWindowTitle("参数编辑");
                    dlg.setLabelText("组件名：");
                    dlg.setInputMode(QInputDialog::TextInput);
                    if( dlg.exec() == QInputDialog::Accepted )
                    {
                        Compname = dlg.textValue();
                    }
                    else
                    {
                        qDebug() << "取消";
                        flag = 1;
                        break;
                    }
                }
                // 没点取消
                if(flag == 0)
                {
                    QListWidgetItem *item1 = new QListWidgetItem();
                    item1->setIcon(QIcon(":/img/component.png"));
                    item1->setText(tr(Compname.toUtf8().data()));
                    //这里的用户角色存储用户数据
                    item1->setData(Qt::UserRole, QPixmap(":/img/component.png"));
                    item1->setData(Qt::UserRole + 1, Compname);
                    item1->setData(Qt::UserRole + 2, this->count());
                    item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
                    this->addDragItem(item1);
                    emit add_one_Comp(Compname);
                }
#endif
                break;
            }
            case 1:
                // 导入文件
                QString dirpath = QDir::currentPath();
                Utils::openDirOrCreate(dirpath);
                // 打开xml文件读取
                const QString fileName = QFileDialog::getOpenFileName(this, tr("打开组件xml"), QString(dirpath), tr("xml files (*.xml)"));
                qDebug() << "长名字：" << fileName;
                //                QFileInfo fi = QFileInfo(fileName);
                //                QString file_name = fi.fileName().split(".")[0];  //获取文件名
                //                qDebug() << "短名字： " << file_name;
                AlgorithmComp ac = Utils::readPluginXmlFile(fileName);
                QString file_name = ac.getFileName();
                // 已经有了
                if(nameList.contains(file_name))
                {
                    qDebug() << "已经导入了! id:" << ac.getInfo()["ID"] << "组件名字: " << ac.getInfo()["Name"];
                    QMessageBox::warning(nullptr, "提醒",
                                         "不能导入具有相同名字的组件入库！",
                                         QMessageBox::Yes);
                }
                else
                {
                    // 将这里的文件复制过来
                    QFile::copy(fileName, dirpath + "/algoXml/" + file_name + ".xml");
                    algorithms.insert(ac.getInfo()["ID"], ac);
                    QListWidgetItem *item1 = new QListWidgetItem();
                    item1->setIcon(QIcon(":/img/component.png"));
                    item1->setText(file_name); //ac.getInfo()["Name"].toUtf8().data()
                    //这里的用户角色存储用户数据
                    item1->setData(Qt::UserRole, QPixmap(":/img/component.png"));
                    item1->setData(Qt::UserRole + 1, file_name); // ac.getInfo()["Name"]
                    // TODO 向这里的id和之前写的id全要改，以xml中的id为准，唯一标识
                    item1->setData(Qt::UserRole + 2, ac.getInfo()["ID"]);
                    item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
                    this->addDragItem(item1);
                    emit add_one_Comp(ac);
                    nameList.append(file_name);
                    qDebug() << "列表的大小：" << algorithms.size();
                }
                break;
            }
        }
        else if(m_dragItem)
        {
            // 这里的ID也不对
            //            int id = qvariant_cast<int>(m_dragItem->data(Qt::UserRole+2))-1; //从0开始
            //            int id = qvariant_cast<int>(m_dragItem->data(Qt::UserRole+2)); //从0开始
            // 改为uuid
            QString id = qvariant_cast<QString>(m_dragItem->data(Qt::UserRole + 2));
            qDebug() << "当前组件的id:" << id;
            // 点击其他组件,这个id应该为xml中的ID
            //            emit add_one_Comp(algorithms[QString::number(id)]);
            emit add_one_Comp(algorithms[id]);
            emit addAlgo2Scene(algorithms[id]);
            // 获取点击的是哪个组件的id，传到radarScene中，知道该渲染出哪个组件
            //            emit setComp_typeandMode(id);

            // UserRole+1是iconName
            emit setComp_typeandMode(qvariant_cast<QString>(m_dragItem->data(Qt::UserRole + 1)), RadarScene::InsertItem);

            qDebug() << "准备拖动!!!" << qvariant_cast<QString>(m_dragItem->data(Qt::UserRole + 1));
        }
    }
    //保留原QListWidget部件的鼠标点击操作.
    QListWidget::mousePressEvent(event);
}


//void RadarCompDraglistWidget::mouseHoverEvent(QMouseEvent *event)
//{
//    //setCursor(Qt::OpenHandCursor);
//}

//void RadarCompDraglistWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::LeftButton)
//    {

//    }
//}



//16777215
