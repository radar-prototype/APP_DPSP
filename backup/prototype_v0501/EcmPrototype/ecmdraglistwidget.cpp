#include "ecmdraglistwidget.h"


ECMDragListWidget::ECMDragListWidget(QWidget *parent) : QListWidget(parent)
{
    //    设置允许接收拖拽
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setSpacing(5);

    //icon图标显示
    setViewMode(QListView::ListMode);
    //TODO 设置ICON大小
    setIconSize(QSize(50, 50));

    this->setDropIndicatorShown(true);
    //设置拖放模式为移动项目，否则为复制项目
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setAttribute(Qt::WA_PendingMoveEvent);

//    加入新建电子对抗按钮项
    this->addItem("新建电子对抗");
    addECMButton = this->item(0);
    addECMButton->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    addECMButton->setBackgroundColor(QColor(211,211,211));
    addECMButton->setFlags(Qt::NoItemFlags);
    addECMButton->setIcon(QIcon(":/img/newradar.png"));

//    设置初始尺寸
    this->resize(250,200);

}

//添加拖拽项，使用此方法会动态resize整个widget
void ECMDragListWidget::addDragItem(QListWidgetItem*item){

    int count = this->count();
    int height = count*40;
    int minHeight = 150;
    int maxHeight = 400;
    if(height < minHeight){
        height = minHeight;
    }
    else if(height > maxHeight){
        height = maxHeight;
    }
    this->resize(240,height);
    this->addItem(item);
}

//添加拖拽项，使用此方法会动态resize整个widget
void ECMDragListWidget::addDragItem(const QString &label){
    int count = this->count();
    int height = count*40;
    int minHeight = 150;
    int maxHeight = 400;
    if(height < minHeight){
        height = minHeight;
    }
    else if(height > maxHeight){
        height = maxHeight;
    }
    this->resize(240,height);
    this->addItem(label);

}

void ECMDragListWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    //可以直接使用currentItem获得当前的组件
    QListWidgetItem *item = currentItem();
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    //分别从用户角色中获取信息
    QPixmap pixmap = qvariant_cast<QPixmap>(item->data(Qt::UserRole));
    QString str_name = qvariant_cast<QString>(item->data(Qt::UserRole+1));

    //设置dataStream
    dataStream << pixmap << str_name;
    qDebug() << "pixmap: " << pixmap;
    qDebug() << "picture str_name: " << str_name;

    QMimeData *mimeData = new QMimeData;
    //设置自定义mime数据
    mimeData->setData(ECMDragListWidget::puzzleMimeType(), itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    //鼠标始终保持在元件的中心
    drag->setHotSpot(QPoint(25, 25));
    drag->setPixmap(pixmap.scaled(50, 50, Qt::KeepAspectRatio));

    //拖动而不是复制
    if (drag->exec(Qt::MoveAction) == Qt::CopyAction){
        //删除掉当前组件
//        delete takeItem(row(item));
        // FIXME 这功能太老了，删除与否还不确定 未来保留组件库的元素
        qDebug() << "复制动作";
    }
}

//重写鼠标点击操作.
void ECMDragListWidget::mousePressEvent(QMouseEvent *event)
{
    //确保左键拖拽.
    if (event->button() == Qt::LeftButton)
    {
//        qDebug() << "item被点击";
        //先保存拖拽的起点.
        m_dragPoint = event->pos();
        //保留被拖拽的项.
        m_dragItem = this->itemAt(event->pos());
//        如果点击项是新建项则新建对抗
        if(m_dragItem == addECMButton){
            int count = this->count();
            QString newname = "对抗"+QString::number(count);
            QListWidgetItem *item1 = new QListWidgetItem();
            item1->setIcon(QIcon(":/img/ecmdemo.jpg"));
            item1->setText(tr(newname.toUtf8().data()));
            //这里的用户角色存储用户数据
            item1->setData(Qt::UserRole, QPixmap(":/img/ecmdemo.jpg"));
            item1->setData(Qt::UserRole+1, newname);
            item1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
//            this->addDragItem(item1);
            this->addItem(item1);
        }
    }
    //保留原QListWidget部件的鼠标点击操作.
    QListWidget::mousePressEvent(event);
}


