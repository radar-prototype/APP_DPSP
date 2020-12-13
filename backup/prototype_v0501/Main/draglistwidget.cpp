#include "draglistwidget.h"
#include <QDrag>
#include <QMessageBox>
#include <QAbstractButton>
#include <ashowdialog.h>
#include "ppidialog.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include "utils.h"
#include "page_structure/left_side_switch/left_utils.h"
/**
* @projectName   prototype_v0719
* @brief         整体工程项目左边的拖拽列表：雷达/对抗/目标设备，是以list的形式表现的。
* @author        Antrn
* @date          2019-08-12
*/
DragListWidget::DragListWidget(QWidget *parent ) : QListWidget(parent)
{
    //    设置允许接收拖拽
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setSpacing(5);
    //icon图标显示
    this->setViewMode(QListView::ListMode);
    //、设置ICON大小
    this-> setIconSize(QSize(40, 40));
    this->setDropIndicatorShown(true);
    //设置拖放模式为移动项目，否则为复制项目
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setAttribute(Qt::WA_PendingMoveEvent);

#if 0
    //旧代码
    this->addItem(tr("新建雷达"));
    addRadarButton = this->item(0);
    addRadarButton->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    addRadarButton->setBackgroundColor(QColor(211, 211, 211));
    addRadarButton->setFlags(Qt::NoItemFlags);
    addRadarButton->setIcon(QIcon(":/img/newradar.png"));

    // 加入新建雷达按钮项
    listItem_add("雷达");
#endif
    // 修改名称后失去焦点生效
    connect(this, &DragListWidget::itemChanged, this, &DragListWidget::renameSlot);
}


/**
 * @brief 添加一个item
 * @param item
 */
void DragListWidget::addDragItem(QListWidgetItem*item)
{
    this->addItem(item);
}

/**
 * @brief 向类终对象插入键值对
 * @return
 */
void DragListWidget::insert2id_item(QString id, QListWidgetItem *item)
{
    this->id_item.insert(id, item);
}

#if 0
// 最古老的代码，用不到
// 添加拖拽项，使用此方法会动态resize整个widget
void DragListWidget::addDragItem(const QString &label)
{
    int count = this->count();
    int height = count * 40;
    int minHeight = 150;
    int maxHeight = 400;
    if(height < minHeight)
    {
        height = minHeight;
    }
    else if(height > maxHeight)
    {
        height = maxHeight;
    }
    this->resize(240, height);
    this->addItem(label);
//    我发现隐藏显示以后，尺寸会变成layout内的默认尺寸，相当于 resize无效，这个方法废了喝喝
    this->setVisible(false);
    this->setVisible(true);
}
#endif

void DragListWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    //可以直接使用currentItem获得当前的组件
    QListWidgetItem *item = currentItem();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    //分别从用户角色中获取信息
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    QString str_name = qvariant_cast<QString>(item->data(Qt::UserRole + 1));
    QString itemType = qvariant_cast<QString>(item->data(Qt::UserRole + 2));
    //设置dataStream
    dataStream << pixmap << str_name << itemType;
//    qDebug() << "pixma    p: " << pixmap;
//    qDebug() << "picture str_name: " << str_name;

    QMimeData *mimeData = new QMimeData;
    //设置自定义mime数据
    mimeData->setData(DragListWidget::puzzleMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //鼠标始终保持在元件的中心
    drag->setHotSpot(QPoint(25, 25));
    drag->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));

    //拖动即是复制一份
    if (drag->exec(Qt::MoveAction) == Qt::CopyAction)
    {
        qDebug() << "复制组件库中的组件的动作";
    }
}
//重写鼠标点击操作.
void DragListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem * item = new QListWidgetItem();
    item = this->itemAt(event->pos());
    //确保左键拖拽.
    if (event->button() == Qt::LeftButton)
    {
//        qDebug() << "item被点击";
        //先保存拖拽的起点.
        m_dragPoint = event->pos();
        //保留被拖拽的项.
        m_dragItem = item;

#if 0
        // 老代码
        // 如果点击项是新建项则新建雷达
        if(m_dragItem == addRadarButton)
        {
            int count = this->count();
            QString newname = "雷达" + QString::number(count);
            QListWidgetItem *item1 = new QListWidgetItem();
            item1->setIcon(QIcon(":/img/radar.png"));
            item1->setText(tr(newname.toUtf8().data()));
            //这里的用户角色存储用户数据
            item1->setData(Qt::UserRole, QPixmap(":/img/radar.png"));
            item1->setData(Qt::UserRole + 1, newname);
            item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
            this->addDragItem(item1);
        }
#endif
    }

    if (event->button() == Qt::RightButton && item != this->item(0))
    {
        if(item)
        {
            item->checkState();
            QString id = this->itemAt(event->pos())->text();

            Menu_iteamOperation *menu = new Menu_iteamOperation();
            connect(menu, &Menu_iteamOperation::itemOperate,
                    [ = ](Menu_iteamOperation::OperateType operate)
            {
                //信号---->信号（向父容器传递）
                QString equipName;
                if(this->objectName().compare("listWidget_rader") == 0)
                    equipName = "radar";
                if(this->objectName().compare("listWidget_ecm") == 0)
                    equipName = "ecm";
                if(this->objectName().compare("listWidget_object") == 0)
                    equipName = "object";

                //确定才删除
                if(operate == Menu_iteamOperation::del)
                {
                    //先询问是否确定
                    if(QMessageBox::Ok == QMessageBox::question(this, "question",
                            "删除此控件，场景中已经添加的组件"
                            "也将被删除！是否还要继续？",
                            QMessageBox::Ok, QMessageBox::Cancel))
                        emit itemOperate(Menu_iteamOperation::del, equipName, id);
                }
                else
                {
                    emit itemOperate(operate, equipName, id);//仅仅向父组件发送信号，没有直接调用自己的函数
                }
            });
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->exec(QPoint(event->globalPos().x(), event->globalPos().y()));
            delete menu;
        }
    }

    //保留原QListWidget部件的鼠标点击操作.
    QListWidget::mousePressEvent(event);
}

void DragListWidget:: mouseDoubleClickEvent(QMouseEvent *event)
{
//  必须新建一个QListWidgetItem过渡 否则点击的地方没有item就死了。
    QListWidgetItem * item = new QListWidgetItem();
    item = this->itemAt(event->pos());
    if(item)
    {
        //双击修改名称
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
        //双击进入编辑 暂时弃用
//        this->itemOperateSlot(Menu_iteamOperation::edit,item->text());
    }
    QListWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief 重命名的处理方法
 * @param item
 */
void DragListWidget::renameSlot(QListWidgetItem* item)
{
    //修改名称的过程会触发两次这个slot，所以第一次用来存初原来的名字，第二次用来改名字
    QString newName = item->text();
    //在修改名字
    if(this->forRename.ifEnableChange)
    {
        if(this->currentRow() == forRename.preIndex && newName != forRename.preName)
        {
            qDebug() << "##newName:" << newName << "preName:" << forRename.preName;
            //校验新名字
            if(!this->id_item.contains(newName))
            {
                //不重复，修改其他所关联项目的id
                qDebug() << "都去改吧！！:1：xml:2：编辑雷达页面 ，3：id_item map数组，4：newedititem数组，5：雷达场景id";
                //交由父类统一下发修改信号
                //itemOperateSlot(Menu_iteamOperation::rename,forRename.preName,newName);
                forRename.ifEnableChange = false;
                emit itemOperate(Menu_iteamOperation::rename, forRename.preName, newName);

            }
            else
            {
                QMessageBox::warning(this, "warning", "名称已存在，请换一                       个名字重新修改", QMessageBox::Ok);
                item->setText(forRename.preName);
//             forRename.ifEnableChange=false;
            }
        }
        forRename.preIndex = this->currentRow();
        forRename.preName =  this->currentItem()->text();
    }
    else
    {
        forRename.ifEnableChange = true;
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief 各种对于列表的操作集合入此方法,目前所有设备都公用mainWindowRadar了
 * @param operateType
 * @param id
 * @param newName
 */
void DragListWidget::itemOperateSlot(Menu_iteamOperation::OperateType operateType, QString id, QString newName)
{
    QListWidgetItem *item = id_item.find(id).value();
    // 触发就选中 Antrn:？？？
//        item->isSelected();
    switch (operateType)
    {
    case Menu_iteamOperation::del:
    {
        qDebug() << "left_delete:" << id;
        if(newEditWindowList.contains(id))
        {
            MainWindow_Radar *radar = newEditWindowList.find(id).value();
            connect(radar, &MainWindow_Radar::iClose, [ =, &item]()
            {
                this->removeItemWidget(item);
                id_item.remove(id_item.key(item));
                //删除后必须更新它
                this->forRename.preIndex = -1;
            });
            radar->close();
            // Antrn: 防止内存泄漏和野指针
            radar->deleteLater();
            radar = nullptr;
        }
        else
        {
            this->removeItemWidget(item);
            id_item.remove(id_item.key(item));
//                   qDebug() << "看看是不是删掉了" << this->objectName() << id_item;
            //删除后必须跟新它
            this->forRename.preIndex = -1;
        }
        delete item;
        break;
    }
    case Menu_iteamOperation::edit:
    {
        qDebug() << "left_edit:" << id;
        //新建或者提升编辑窗口
        {
            //查找是否已经创建该子类
            if(!newEditWindowList.contains(id))
            {
                // 新建(每个变量命名不同)
                // [注意] 这里共用了雷达的建模编辑窗口！！
                MainWindow_Radar *SET_RADARNAME(id) = new MainWindow_Radar(id);
                newEditWindowList.insert(id, SET_RADARNAME(id));
                //窗口关闭时更新子类列表：newEditWindowList
                connect(SET_RADARNAME(id), &MainWindow_Radar::iClose,
                        [ = ](MainWindow_Radar * radar)
                {
                    newEditWindowList.remove(radar->getEquip_id());
                });
                SET_RADARNAME(id)->setAttribute(Qt::WA_DeleteOnClose);
//                     SET_RADARNAME(id)->setWindowIcon(QIcon(":/img/radar.png"));
//                     SET_RADARNAME(id)->show();
                emit set_window( Left_utils::development_board, Left_utils::switch_update, SET_RADARNAME(id));
            }
            else
            {
                //获取改item对应的mainwindow_radar
                MainWindow_Radar*radar = newEditWindowList.find(id).value();
//                    QMessageBox::warning(this,"警告","你已经打开了建模编辑窗口，不可以重复打开！",QMessageBox::Ok);
                emit set_window( Left_utils::development_board, Left_utils::switch_update, radar);
//                    radar->showNormal();
//                    radar->raise();
//                    radar->showMaximized();
            }
        }
        break;
    }
    case Menu_iteamOperation::rename:
    {
        if(  this->id_item.contains(id))
        {
            this->id_item.insert(newName, this->id_item.find(id).value());
            this->id_item.remove(id);
        }
        if(  this->newEditWindowList.contains(id))
        {
            MainWindow_Radar *radar = this->newEditWindowList.find(id).value();
            radar->setEquip_id(newName);
            this->newEditWindowList.insert(newName, radar);
            this->newEditWindowList.remove(id);
            radar->setWindowTitle(tr((newName + " Edit").toUtf8().data()));
        }
        item->setData(Qt::UserRole + 1, newName);
        break;
    }
    case Menu_iteamOperation::property:
        qDebug() << "left_property:" << id;
        break;
    case Menu_iteamOperation::ppi:
    {
        qDebug() << "ppi show:" << id;
        PPIDialog ppid;
        ppid.exec();
        break;
    }
    case Menu_iteamOperation::ashow:
    {
        qDebug() << "A show:" << id;
        AshowDialog ashow;
        ashow.exec();
    }
    }
}

/**
 * @brief 左边dock的list列表增加item
 * @param name
 */
void DragListWidget::listItem_add(QString name)
{
    // 添加最上方的添加按钮
    QListWidgetItem *listItem_top = new QListWidgetItem();
    listItem_top->setFlags(Qt::NoItemFlags);
    listItem_top->setWhatsThis("添加更多的" + name + "组件");
    listItem_top->setText("添加" + name);
    listItem_top->setIcon(QIcon(":/img/newradar.png"));
    this->addItem(listItem_top);
    // 定义信号和槽
    connect(this, &QListWidget::itemClicked, [ = ](QListWidgetItem * action_item)
    {
        if (action_item->text() == "添加" + name)
        {
//            qDebug()<<"触发新建item了。";
            QMessageBox *msgBox = new QMessageBox(nullptr);
            // 加上这一行之后不提示setGeometry: Unable to set geometry 640x480+821+463...警告，但是会偏离中心，靠左上角
//            msgBox.setGeometry((QApplication::desktop()->width() - msgBox.width())/2,(QApplication::desktop()->height() - msgBox.height())/2, msgBox.width(), msgBox.width());
            msgBox->setWindowTitle("添加" + name);
            msgBox->setText("添加" + name);
            msgBox->setInformativeText("您想要创建一个新的" + name + "组件，还是导入一个已经有的" + name + "组件？");
            QPushButton*newButton = msgBox->addButton(tr("新建"), QMessageBox::ActionRole);
            msgBox->addButton(tr("导入"), QMessageBox::ActionRole);
            msgBox->addButton(tr("取消"), QMessageBox::RejectRole);
            msgBox->setDefaultButton(newButton);
            int button_index = msgBox->exec();

            switch (button_index)
            {
            case 0:
            {
                // 新建文件
                Utils::importXml(this, &id_inde, &nameSet, &id_item, 1, name);
                break;
            }
            case 1:
                // 导入文件, 打开文件读取
                Utils::importXml(this, &id_inde, &nameSet, &id_item, 2, "");
                break;
            case 2:
                break;
            }
        }
    });
}

QString DragListWidget::getTarType() const
{
    return tarType;
}

QString DragListWidget::getEcmType() const
{
    return ecmType;
}

QString DragListWidget::getRadarType() const
{
    return radarType;
}

QSet<QString> DragListWidget::getNameSet() const
{
    return nameSet;
}

void DragListWidget::add2NameSet(QString newName)
{
    this->nameSet.insert(newName);
}

#if 0
/**
 * @brief 朝左边list列表添加元素
 * @param name
 */
void DragListWidget::add_listItem(QString name)
{
    QString path = ":/img/radar.png";
    // FIXME 有必要枚举 待修缮
    int fileCount = id_inde;
    // 不同目标的文件夹
    QString path_;
    if(name == "雷达")
    {
        path = ":/img/radar.png";
        path_ = QDir::currentPath() + "/radar";
    }
    else if(name == "对抗")
    {
        path = ":/img/ECM.png";
        path_ = QDir::currentPath() + "/ecm";
    }
    else if(name == "目标")
    {
        path = ":/img/object.png";
        path_ = QDir::currentPath() + "/object";
    }
    // 先检查每种对象分别有几个已存在
    QDir dir(path_);
    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach ( QFileInfo fileInfo, fileInfoList )
    {
        if ( fileInfo.fileName() == "." || fileInfo.fileName() == ".." )
            continue;
        else if ( fileInfo.isDir() )
        {
            nameSet.insert(fileInfo.fileName());
            fileCount++;
        }
        else
        {
            continue;
        }
    }
    if(fileCount != 0)
    {
        id_inde = fileCount;
    }
    //新建item，添加到左边的listwidget
    QString newName = name + QString::number(id_inde++);

//    while(this->id_item.contains(newName))
    while(nameSet.contains(newName))
    {
        //名称已经存在，换一个
        newName = name + QString::number(id_inde++);
    }
    nameSet.insert(newName);
    QListWidgetItem *item = new QListWidgetItem();
    id_item.insert(newName, item);
    item->setIcon(QIcon(path));
    item->setText(tr(newName.toUtf8().data()));

    //这里的用户角色存储用户数据（和拖入场景有关）
    item->setData(Qt::UserRole, QPixmap(path));
    item->setData(Qt::UserRole + 1, newName);
    item->setData(Qt::UserRole + 2, itemType);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
    this->addDragItem(item);
    qDebug() << "additem了, name:" << newName;
}
#endif

QMap<QString, QListWidgetItem *> DragListWidget::getId_item() const
{
    return id_item;
}

/**
 * @brief 关闭主窗口时会检查一下现在这个
 * @return
 */
bool DragListWidget::closeDragListWidget()
{
    // 如果列表中的编辑窗口都已经销毁了，就返回true
    if(this->newEditWindowList.isEmpty())
        return true;
    else
    {
        QMessageBox::warning(this, "警告", "有子窗口未关闭！请先关闭子窗口。");
        QMap<QString, MainWindow_Radar*>::iterator i = this->newEditWindowList.begin();
        while (i != newEditWindowList.end())
        {
            i.value()->showNormal();
            i.value()->raise();
            i++;
        }
        return false;
    }
}
