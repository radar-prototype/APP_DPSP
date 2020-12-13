#include "arrowitem.h"
#include "draglistwidget.h"
#include "mainwindownewscene.h"
#include "radaritem.h"
#include "ecmitem.h"
#include "utils.h"
#include <QDesktopWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
/**
* @projectName   prototype_v0719
* @brief         新建工程的主场景，用户配置雷达或者电子对抗，用户整体仿真。
* @author        Antrn
* @date          2019-08-12
*/
MainWindowNewScene::MainWindowNewScene()
{
//    将begin_item设为空指针
    line_begin_item = nullptr;
    dragFlag = false;
    // 清空列表
    Item_List.clear();
    graphicsLineItem_List.clear();
//    QGraphicsPixmapItem* bgItem = new QGraphicsPixmapItem(QPixmap(":/img/cn.jpg"));
//    addItem(bgItem);
}

//由两个Radaritem计算起始点和终点
void MainWindowNewScene::computeLine(QGraphicsItem*fromItem, QGraphicsItem*toItem)
{
    QRectF from = QRectF();
    from.setX(fromItem->pos().x());
    from.setY(fromItem->pos().y());
    from.setWidth(fromItem->boundingRect().width());
    from.setHeight(fromItem->boundingRect().height());

    QRectF to = QRectF();
    to.setX(toItem->pos().x());
    to.setY(toItem->pos().y());
    to.setWidth(toItem->boundingRect().width());
    to.setHeight(toItem->boundingRect().height());

    QPointF from_center = QPointF(from.center());
    QPointF from_top = QPointF(from_center.x(), from.top());
    QPointF from_right = QPointF(from.right(), from_center.y());
    QPointF from_bottom = QPointF(from_center.x(), from.bottom());
    QPointF from_left = QPointF(from.left(), from_center.y());

    QPointF to_center = QPointF(to.center());
    QPointF to_top = QPointF(to_center.x(), to.top());
    QPointF to_right = QPointF(to.right(), to_center.y());
    QPointF to_bottom = QPointF(to_center.x(), to.bottom());
    QPointF to_left = QPointF(to.left(), to_center.y());

    QList<QLineF> compare_line_List = QList<QLineF>();
    QLineF minline;

    //    当to在from右上角时
    if(to.left() > from.right() && to.bottom() < from.top())
    {
        QLineF line1 = QLineF(from_top, to_left);
        QLineF line2 = QLineF(from_top, to_bottom);
        QLineF line3 = QLineF(from_right, to_left);
        QLineF line4 = QLineF(from_right, to_bottom);
        compare_line_List.append(line1);
        compare_line_List.append(line2);
        compare_line_List.append(line3);
        compare_line_List.append(line4);
        minline = computeMinLength(compare_line_List);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from右下角时
    if(to.left() > from.right() && to.top() > from.bottom())
    {
        QLineF line1 = QLineF(from_right, to_left);
        QLineF line2 = QLineF(from_right, to_top);
        QLineF line3 = QLineF(from_bottom, to_left);
        QLineF line4 = QLineF(from_bottom, to_top);
        compare_line_List.append(line1);
        compare_line_List.append(line2);
        compare_line_List.append(line3);
        compare_line_List.append(line4);
        minline = computeMinLength(compare_line_List);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from左上角时
    if(to.right() < from.left() && to.bottom() < from.top())
    {
        QLineF line1 = QLineF(from_top, to_right);
        QLineF line2 = QLineF(from_top, to_bottom);
        QLineF line3 = QLineF(from_left, to_right);
        QLineF line4 = QLineF(from_left, to_bottom);
        compare_line_List.append(line1);
        compare_line_List.append(line2);
        compare_line_List.append(line3);
        compare_line_List.append(line4);
        minline = computeMinLength(compare_line_List);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from左下角时
    if(to.right() < from.left() && to.top() > from.bottom())
    {
        QLineF line1 = QLineF(from_left, to_right);
        QLineF line2 = QLineF(from_left, to_top);
        QLineF line3 = QLineF(from_bottom, to_right);
        QLineF line4 = QLineF(from_bottom, to_top);
        compare_line_List.append(line1);
        compare_line_List.append(line2);
        compare_line_List.append(line3);
        compare_line_List.append(line4);
        minline = computeMinLength(compare_line_List);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from上方时
    if(to.left() <= from.right() && to.right() >= from.left() && to.bottom() < from.top())
    {
        minline = QLineF(from_top, to_bottom);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from下方时
    if(to.left() <= from.right() && to.right() >= from.left() && to.top() > from.bottom())
    {
        minline = QLineF(from_bottom, to_top);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from左面时
    if(to.bottom() >= from.top() && to.top() <= from.bottom() && to.right() < from.left())
    {
        minline = QLineF(from_left, to_right);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }
    //    当to在from右面时
    if(to.bottom() >= from.top() && to.top() <= from.bottom() && to.left() > from.right())
    {
        minline = QLineF(from_right, to_left);
        sourcePoint = minline.p1();
        destPoint = minline.p2();
        return;
    }

}

//循环找到最短路径
QLineF MainWindowNewScene::computeMinLength(QList<QLineF> line_list)
{
    QLineF min = line_list.first();
    for(int i = 1; i < line_list.length(); i++)
    {
        if(line_list.value(i).length() < min.length())
        {
            min = line_list.value(i);
        }
    }
    return min;
}


// NOTE 此函数没有起作用
void MainWindowNewScene::LinePaint()
{
    qDebug() << "重绘画板";

}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-20
 * @brief MainWindowNewScene::mousePressEvent 鼠标按下的事件，目前是在场景中拖拽，无法移出场景的空间
 * 想要实现将场景中的组件拖出来自动删掉的功能就要加上本方法尾部的注释掉的那一段，但是这样的话要区分是从外部移进来的drag对象还是内部拖动时
 * 产生的drag对象，如果是内部原来的组件，只是拖动改变位置，那么就得点击的时候新建drag对象，存入data数据，drop的时候重新读一遍
 * 但是这样和外部list组件库中拖拽进来的drag对象的行为产生冲突，当从组件库拖进来多个相同组件的时候存的data信息是一样的，这时候zhang处理的办法是
 * 将后面附加'~'加数字，这样的话，当内部拖动的时候每次也会重新名一次名，每拖动一次名字就变了，这样的话周围的连线信息也会混乱，每点击一下就会新建一个，得像个办法区分一下
 * @param event
 */
void MainWindowNewScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // qDebug() << "鼠标点击位置：" << event->scenePos();
    dragFlag = true;
    // 点到的位置有图元存在
    if(this->itemAt(event->scenePos(), QTransform()))
    {
        emit showParamSettingBtn(true);
    }
    else
    {
        emit showParamSettingBtn(false);
        // 清除起点
        setlineBeginItem(nullptr);
    }
    //确保按住左键移动.
    if (event->buttons() == Qt::LeftButton)
    {
//        startPoint = event->pos();
        //先判断是否有begin_item存在，如果不存在
        if(line_begin_item == nullptr)
        {
            // 点击处如果有item
            if(this->itemAt(event->scenePos(), QTransform()))
            {
//                更新begin_item
                line_begin_item = this->itemAt(event->scenePos(), QTransform());
                // 获取拖拽对象，此处是为拖拽做初始化准备，与连线无关，在mousemove事件做进一步处理
                drag_item = this->itemAt(event->scenePos(), QTransform());
                connect_to_List.clear();
                connect_from_List.clear();
                delete_line_List.clear();
                // 遍历线数组
                for(int i = 0; i < graphicsLineItem_List.length(); i++)
                {
                    ArrowItem*linetemp = graphicsLineItem_List.at(i);
                    if(linetemp->getEndItem() == drag_item)
                    {
                        connect_from_List.append(linetemp->getBeginItem());
                        delete_line_List.append(linetemp);
                    }
                    if(linetemp->getBeginItem() == drag_item)
                    {
                        connect_to_List.append(linetemp->getEndItem());
                        delete_line_List.append(linetemp);
                    }

                }
            }
        }
//        如果begin_item存在
        else
        {
//          第二次点击处如果有item
            if(this->itemAt(event->scenePos(), QTransform()))
            {
                QGraphicsItem *line_end_item = this->itemAt(event->scenePos(), QTransform());
//                如果两次点击了同一item
                if(line_begin_item == line_end_item)
                {
                    // 不连线,退出函数,begin_item不变
                }
                else
                {
                    // 如果两次点击了不同item,开始连线
                    computeLine(line_begin_item, line_end_item);
                    ArrowItem *arrow = new ArrowItem;
                    arrow->setLineItem(line_begin_item, line_end_item);
                    arrow->setData(sourcePoint, destPoint);
                    arrow->setFlag(QGraphicsItem::ItemIsSelectable, true);
                    this->addItem(arrow);
                    this->graphicsLineItem_List.append(arrow);
                    delete_line_List.append(arrow);
                    update();
//                    连线后把begin_item设回空指针
                    line_begin_item = nullptr;
                    connect(arrow, &ArrowItem::destroyed, [ = ]()
                    {
                        // 删除箭头
                        qDebug() << "删除箭头！";
                        removeItem(arrow);
                        graphicsLineItem_List.removeOne(arrow);
                        delete_line_List.removeOne(arrow);
                    });
                }
            }
//          第二次点击处如果没item，放弃连线，并把begin_item设回空指针
            else
            {
                line_begin_item = nullptr;
            }
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void MainWindowNewScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    dragFlag = false;
    QGraphicsScene::mouseReleaseEvent(event);
}

void MainWindowNewScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // 若为拖拽
    if(dragFlag == true)
    {
        line_begin_item = nullptr;

        for(int i = 0; i < delete_line_List.length(); i++)
        {
            this->removeItem(delete_line_List.at(i));
            graphicsLineItem_List.removeOne(delete_line_List.at(i));
        }
        delete_line_List.clear();
        for (int i = 0; i < connect_to_List.length(); i++)
        {
            computeLine(drag_item, connect_to_List.at(i));
            ArrowItem *arrow = new ArrowItem;
            arrow->setLineItem(drag_item, connect_to_List.at(i));
            arrow->setData(sourcePoint, destPoint);
            arrow->setFlag(QGraphicsItem::ItemIsMovable, true);
            this->addItem(arrow);
            this->graphicsLineItem_List.append(arrow);
            delete_line_List.append(arrow);
            connect(arrow, &ArrowItem::destroyed, [ = ]()
            {
                // 删除箭头
                qDebug() << "删除箭头！";
                removeItem(arrow);
                graphicsLineItem_List.removeOne(arrow);
                delete_line_List.removeOne(arrow);
            });
        }
        for (int i = 0; i < connect_from_List.length(); i++)
        {
            computeLine(connect_from_List.at(i), drag_item);
            ArrowItem *arrow = new ArrowItem;
            arrow->setLineItem(connect_from_List.at(i), drag_item);
            arrow->setData(sourcePoint, destPoint);
            arrow->setFlag(QGraphicsItem::ItemIsMovable, true);
            this->addItem(arrow);
            this->graphicsLineItem_List.append(arrow);
            delete_line_List.append(arrow);
            connect(arrow, &ArrowItem::destroyed, [ = ]()
            {
                // 删除箭头
                qDebug() << "删除箭头！";
                removeItem(arrow);
                graphicsLineItem_List.removeOne(arrow);
                delete_line_List.removeOne(arrow);
            });
        }
        update();
    }
    emit signal_xy(event->scenePos().x(), event->scenePos().y());
    QGraphicsScene::mouseMoveEvent(event);
}

void MainWindowNewScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(DragListWidget::puzzleMimeType()))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
//        event->acceptProposedAction();
    else
        event->ignore();
    qDebug() << "元素被托入到场景中";
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindowNewScene::dropEvent 拖入新建工程场景中时的丢下事件
 * @param event
 */
void MainWindowNewScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(DragListWidget::puzzleMimeType()))
    {
        event->acceptProposedAction();

        QByteArray comData = event->mimeData()->data(DragListWidget::puzzleMimeType());
        QDataStream dataStream(&comData, QIODevice::ReadOnly);
        QPixmap pixmap;
        QString name;
        QString itemType;
        dataStream >> pixmap >> name >> itemType;
//        qDebug() << pixmap << " ; str：" << str<< "场景 中的 item 的 ID;
        if(itemType == "雷达")
        {
            QString id;
            RadarItem *radar;
            if(!id_gItem.contains(name))
            {
                id = name;
                radar = new RadarItem(id);
            }
            else
            {
                int i = 1;
                while(id_gItem.contains(name + "~" + QString::number(i)))
                {
                    i++;
                }
                id = name + "~" + QString::number(i);
                radar = new RadarItem(id);
                qDebug() << "多个Item拖入场景 分配name为:" << id;
            }
            connect(radar, &RadarItem::itemOperate, this, [ = ](Menu_iteamOperation::OperateType operateType, QString id)
            {
                QString name = id.split("~").first();
                emit itemOperate(operateType,Page_structure_utils::radar, name);
            });
            // 设置初始位置
            radar->setPos(event->scenePos().x() - 35, event->scenePos().y() - 35);
            qDebug() << "MainWindowNewScene : radar id : " << id;
//            qDebug()<<"放下位置: " <<event->scenePos();

            this->addItem(radar);
            Item_List.append(radar);
            id_gItem.insert(id, radar);
            // 应该抽成函数的吧 Antrn:...知道应该你不抽！shao：我。。。
            connect(radar, &RadarItem::destroyed, [ = ]()
            {
                this->removeItem(radar);
                Item_List.removeOne(radar);
//                删除对应的连线
                delete_line_List.clear();
                connect_to_List.clear();
                connect_from_List.clear();
                for(int i = 0; i < graphicsLineItem_List.length(); i++)
                {
                    ArrowItem*linetemp = graphicsLineItem_List.at(i);
                    if(linetemp->getEndItem() == radar || linetemp->getBeginItem() == radar)
                    {
                        delete_line_List.append(linetemp);
                    }
                }
                for(int i = 0; i < delete_line_List.length(); i++)
                {
                    this->removeItem(delete_line_List.at(i));
                    graphicsLineItem_List.removeOne(delete_line_List.at(i));
                }
                delete_line_List.clear();
            });
        }
        else if(name.startsWith("对抗"))
        {
            QString id;
            ECMItem *ecm = nullptr;
            if(!id_gItem.contains(name))
            {
                id = name;
                ecm = new ECMItem;
            }
            else
            {
                int i = 1;
                while(id_gItem.contains(name + "~" + QString::number(i)))
                {
                    i++;
                }
                id = name + "~" + QString::number(i);
                ecm = new ECMItem;
                qDebug() << "多个Item拖入场景 分配name为:" << id;
            }
            // 设置初始位置
            ecm->setPos(event->scenePos().x() - 35, event->scenePos().y() - 35);
            ecm->setEcm_id(id);
            // TODO ECM好多功能未完成，这是zhang的历史遗留吧？？shao:我。。。
//            connect(ecm,&ECMItem::itemOperate,this,[=](Menu_iteamOperation::OperateType operateType,QString id){
//                QString name=id.split("~").first();
//                emit itemOperate(operateType, "radar", name);
//            });
            this->addItem(ecm);
            Item_List.append(ecm);
            connect(ecm, &ECMItem::destroyed, [ = ]()
            {
                this->removeItem(ecm);
                Item_List.removeOne(ecm);
//                删除对应的连线
                delete_line_List.clear();
                connect_to_List.clear();
                connect_from_List.clear();
                for(int i = 0; i < graphicsLineItem_List.length(); i++)
                {
                    ArrowItem*linetemp = graphicsLineItem_List.at(i);
                    if(linetemp->getEndItem() == ecm || linetemp->getBeginItem() == ecm)
                    {
                        delete_line_List.append(linetemp);
                    }
                }
                for(int i = 0; i < delete_line_List.length(); i++)
                {
                    this->removeItem(delete_line_List.at(i));
                    graphicsLineItem_List.removeOne(delete_line_List.at(i));
                }
                delete_line_List.clear();
            });
        }
        else
        {
            // TODO 目标设备先放着，电子对抗还有一大堆事呢
            Utils::alert(QApplication::desktop()->rect(), "别担心，该功能正在开发中");
        }
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
    //qDebug() << "放下位置: " << event->scenePos() <<endl;
}

void MainWindowNewScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{

    if (event->mimeData()->hasFormat(DragListWidget::puzzleMimeType()))
    {
        //设置为移动而不是复制
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindowNewScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    event->setDropAction(Qt::MoveAction);
    qDebug() << "拖拽出场景";
}

void MainWindowNewScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QPixmap pm(":/img/cn.png");
    //  这一行保证背景图片始终和视图窗口一样大，无论怎样缩放都是固定，比较适合设置固定背景
//    painter->drawPixmap(rect, pm, QRect());
//    qDebug() << pm.width()<< pm.height(); //2220 1615
    painter->drawPixmap(0, 0, pm.width(), pm.height(), pm);
    QGraphicsScene::drawBackground(painter, rect);
}

void MainWindowNewScene::setlineBeginItem(QGraphicsItem *value)
{
    line_begin_item = value;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-19
 * @brief MainWindowNewScene::itemOperateSlot 操作图元的方法
 * @param operate 操作类别
 * @param type 设备种类
 * @param name 设备名字
 * @param newName 新名字
 */
void MainWindowNewScene::itemOperateSlot(Menu_iteamOperation::OperateType operate,  Page_structure_utils::window_type list_type, QString name, QString newName)
{
    // 说明已经拖入了至少一个 Antrn:要是一个也没呢？？？shao:一个也没有就不用管了呀
    if(id_gItem.contains(name))
    {
        int i = 1;
        // 确定有几个同名item被拖入
        while(id_gItem.contains(name + "~" + QString::number(i)))
        {
            i++;
        }
        QString id;
        switch(operate)
        {
            //仅仅适用于左边菜单栏下达的删除指令，把场景中的所有相关图元删掉
            case Menu_iteamOperation::del:
            {
                for(int j = 0; j < i; j++)
                {
                    id = j == 0 ? name : name + "~" + QString::number(j);
                    if(list_type == Page_structure_utils::radar)
                    {
                        RadarItem *item;
                        item = qgraphicsitem_cast<RadarItem*>(id_gItem.find(id).value());
                        item->destroyed();
                        this->removeItem(item);
                        Item_List.removeOne(item);
                        id_gItem.remove(id);
                    }
                    else if(list_type == Page_structure_utils::ecm)
                    {
                        ECMItem *item;
                        item = qgraphicsitem_cast<ECMItem*>(id_gItem.find(id).value());
                        item->destroyed();
                        this->removeItem(item);
                        Item_List.removeOne(item);
                        id_gItem.remove(id);
                    }
                    else if(list_type == Page_structure_utils::object)
                    {
                        // TODO 待完成
                    }

                }
                break;
            }
            case Menu_iteamOperation::rename:
            {
                QString id_new;
                RadarItem *item;
                ECMItem *ecm;
                for(int j = 0; j < i; j++)
                {
                    id = j == 0 ? name : name + "~" + QString::number(j);
                    id_new = j == 0 ? newName : newName + "~" + QString::number(j);
                    if(list_type == Page_structure_utils::radar)
                    {
                        item = qgraphicsitem_cast<RadarItem*>(id_gItem.find(id).value());
                        item->itemOperateSlot(Menu_iteamOperation::rename, id_new);
                        this->id_gItem.insert(id_new, item);
                        this->id_gItem.remove(id);
                    }
                    else if (list_type == Page_structure_utils::ecm)
                    {
                        ecm = qgraphicsitem_cast<ECMItem*>(id_gItem.find(id).value());
                        // FIXME 电子对抗设备没实现重命名
    //                        ecm->itemOperateSlot(Menu_iteamOperation::rename,id_new);
                        this->id_gItem.insert(id_new, ecm);
                        this->id_gItem.remove(id);
                    }
                    else if(list_type == Page_structure_utils::object)
                    {
                        // TODO 完善目标设备逻辑
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}
