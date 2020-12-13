#include "main_left_list.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopWidget>
#include "utils.h"
#include <QApplication>
#include "page_structure/left_side_switch/left_utils.h"
#include "ppidialog.h"
#include "ashowdialog.h"
Main_left_list::Main_left_list(QWidget *parent ) : QListWidget(parent)
{
  //设置允许接收拖拽
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->setSpacing(5);
  //icon图标显示
  this->setViewMode(QListView::ListMode);
  //设置ICON大小
  this-> setIconSize(QSize(40, 40));
  this->setDropIndicatorShown(true);
  //设置拖放模式为移动项目，否则为复制项目
  this->setDragDropMode(QAbstractItemView::InternalMove);
  this->setAttribute(Qt::WA_PendingMoveEvent);
  //shao:列表里的添加按钮
  this->listItem_add_button();
}

void Main_left_list::startDrag(Qt::DropActions /*supportedActions*/)
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
    mimeData->setData(Main_left_list::puzzleMimeType(), itemData);

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
void Main_left_list::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem* item = this->itemAt(event->pos());
    //确保左键拖拽.
    if (event->button() == Qt::LeftButton)
    {
//        qDebug() << "item被点击";
        //先保存拖拽的起点.
        m_dragPoint = event->pos();
        //保留被拖拽的项.
        m_dragItem = item;
    }
    if (event->button() == Qt::RightButton && item != this->item(0))
    {
        if(item)
        {
            item->checkState();
            QString item_name = item->text();
            Menu_iteamOperation *menu = new Menu_iteamOperation();
            connect(menu, &Menu_iteamOperation::itemOperate,
                    [ = ](Menu_iteamOperation::OperateType operate) {
                //信号---->信号（仅仅向父容器传递，先不触发自己的槽函数）
            emit itemOperate(operate,  this->list_type, item_name);//仅仅向父组件发送信号，没有直接调用自己的函数
            });
            menu->setAttribute(Qt::WA_DeleteOnClose);
            menu->exec(QPoint(event->globalPos().x(), event->globalPos().y()));
            delete menu;
        }
    }
    //保留原QListWidget部件的鼠标点击操作.
    QListWidget::mousePressEvent(event);
}

//shao：不能改名字了，改名字用右击菜单，现在是直接打开编辑
void Main_left_list:: mouseDoubleClickEvent(QMouseEvent *event)
{
    QListWidgetItem * item = this->itemAt(event->pos());
    if(item)
    {
        //双击进入编辑
        this->itemOperate(Menu_iteamOperation::edit,this->list_type,item->text());
    }
    QListWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief 左边dock的list列表增加item
 * @param name
 */
void Main_left_list::listItem_add_button()
{
    QString name=this->equipName;
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
        this->set_list_type();
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

            QMainWindow* window;
            QString item_name;
            switch (button_index)
            {
            case 0:
            {
                // 新建文件
//                window=Store::add_radar_window();
                window=Store::add_window(this->list_type);
                if(window==nullptr) return;
                 item_name=window->windowTitle();
                break;
            }
            case 1:
            {
                QString type;
                if(this->list_type==Page_structure_utils::radar)type="radar";
                else if(this->list_type==Page_structure_utils::ecm)type="ecm";
                else if(this->list_type==Page_structure_utils::object)type="object";
                // 导入文件, 打开文件读取
                QString path= QFileDialog::getOpenFileName(nullptr, "打开rad/ecm/targ工程", QString(QDir::currentPath())+"/"+type, "files (*.rad *.ecm *.targ)");
//                window=Store::open_radar_window(path);
                if(path.isEmpty())return;
                window=Store::open_window(this->list_type,path);
                if(window==nullptr)return;
                 item_name=window->windowTitle();
                break;
            }
            case 2:
                return;
            }
            this->add_listWidgetItem(item_name);
            //shao：仔细数了一下第7层的就是 mainwindow，总觉得不是个好方法，但是目前我只能想到这个了
            dynamic_cast<MainWindow*>(this->parent()->parent()->parent()->parent()->parent()->parent()->parent())->isSave=false;
        }
    });
}
//添加item
void Main_left_list::add_listWidgetItem(QString  item_name){
    if(this->findItems(item_name,Qt::MatchFixedString).length()>0) {
//        已经存在
       Utils::alert(qApp->desktop()->rect(), "这个eitem 已经存在了。");
        return;
    }

    QString path;
    QString path_;

    this->set_list_type();

    if(this->list_type==Page_structure_utils::radar)
    {
        path = ":/resources/img/radar.png";
        path_ = QDir::currentPath() + "/radar/";
    }
    else if(this->list_type==Page_structure_utils::ecm)
    {
        path = ":/resources/img/ECM.png";
        path_ = QDir::currentPath() + "/ecm/";
    }
    else if(this->list_type==Page_structure_utils::object)
    {
        path = ":/resources/img/object.png";
        path_ = QDir::currentPath() + "/object/";
    }

     // 添加到左边的listwidget
        QListWidgetItem *item = new QListWidgetItem();
        item->setIcon(QIcon(path));
        item->setText(item_name);

        //这里的用户角色存储用户数据（和拖入场景有关）
        item->setData(Qt::UserRole, QPixmap(path));
        item->setData(Qt::UserRole + 1, item_name);
        item->setData(Qt::UserRole + 2, this->equipName);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);

        this->addItem(item);
}


// This is an auto-generated comment.
/**
 * @brief 各种对于列表的操作集合入此方法
 */
void Main_left_list::itemOperateSlot(Menu_iteamOperation::OperateType operateType,Page_structure_utils::window_type list_type,QString item_name,QString new_name)
{
    //shao:不是自己的信号就都不要
    if(list_type!=this->list_type) return;
   QListWidgetItem *item =this->findItems(item_name,Qt::MatchFixedString).first();
    switch (operateType)
    {
        case Menu_iteamOperation::del:
        {
//           this->removeItemWidget(item);
            delete item;
        //shao：仔细数了一下第7层的就是 mainwindow，总觉得不是个好方法，但是目前我只能想到这个了
        dynamic_cast<MainWindow*>(this->parent()->parent()->parent()->parent()->parent()->parent()->parent())->isSave=false;
            break;
        }
        case Menu_iteamOperation::edit:
        {
            qDebug() << "left_edit:" << item_name;
            break;
        }
        case Menu_iteamOperation::rename:
        {
            item->setData(Qt::UserRole + 1, new_name);
            item->setText(new_name);
            break;
        }
        case Menu_iteamOperation::property:
        {
            qDebug() << "left_property:" << item_name;
            break;
        }
        case Menu_iteamOperation::ppi:
        {
//            qDebug() << "ppi show:" << item_name;
            PPIDialog ppid;
            ppid.exec();
            break;
        }
        case Menu_iteamOperation::ashow:
        {
//            qDebug() << "A show:" << item_name;
            AshowDialog ashow;
            ashow.exec();
        }
    }
}
void Main_left_list::set_list_type(){
    //shao：确定是哪一个list(原来写在构造函数里没用)
    if(this->objectName().compare("listWidget_rader") == 0){
        this->list_type = Page_structure_utils::radar;this->equipName="雷达";}
    if(this->objectName().compare("listWidget_ecm") == 0){
        this->list_type =Page_structure_utils::ecm;this->equipName="对抗样机";}
    if(this->objectName().compare("listWidget_object") == 0){
        this->list_type = Page_structure_utils::object;this->equipName="目标样机";}
}
