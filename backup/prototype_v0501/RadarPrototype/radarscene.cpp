#include "radarscene.h"
#include "arrow.h"
#include <QDebug>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QMetaEnum>
#include <runcompconf.h>
#include <QTimer>
#include "diagramitem.h"
#include "mainwindow_radar.h"
#include "radarcompdraglistwidget.h"
#include "utils.h"
#include "itemport.h"
#include "development_panel/signalshow/realtimesignal.h"
#include <QCursor>
/**
* @projectName   prototype_v0719
* @brief         编辑雷达页面的自定义场景类，上面绘制设计雷达的组件链接。
* @author        Antrn
* @date          2019-08-12
*/
//Q_DECLARE_METATYPE(DiagramItem)
RadarScene::RadarScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;

    //    line = nullptr;
    conn = nullptr;
    textItem = nullptr;
    myItemColor = Qt::gray;
    myTextColor = Qt::black;
    myLineColor = Qt::black;

    //添加处理指令即xml头部说明和场景的属性
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("components");
    doc.appendChild(root);
    QDomAttr scene_w = doc.createAttribute("width");
    QDomAttr scene_h = doc.createAttribute("height");
    scene_w.setValue(QString::number(width()));
    scene_h.setValue(QString::number(height()));
    root.setAttributeNode(scene_w);
    root.setAttributeNode(scene_h);
    Arrs = doc.createElement("Arrs");
    root.appendChild(Arrs);
    Items = doc.createElement("Items");
    root.appendChild(Items);
    rubberBand = nullptr;
}

//QGraphicsItem *RadarScene::itemAt(const QPointF &pos)
//{
//    QList<QGraphicsItem*> items = this->items(QRectF(pos - QPointF(1,1), QSize(3,3)));
//    foreach(QGraphicsItem *item, items)
//        if (item->type() > QGraphicsItem::UserType)
//            return item;
//    return nullptr;
//}


void RadarScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    //只过滤箭头设置颜色
    if (isItemChange(Arrow::Type))
    {
        //一次只能设置一个
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

void RadarScene::setTextColor(const QColor &color)
{
    myTextColor = color;
    if (isItemChange(DiagramTextItem::Type))
    {
        //一次只能设置一个
        DiagramTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        item->setDefaultTextColor(myTextColor);
    }
}

// 好像不管用了
void RadarScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type))
    {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        // 填充
        item->setBrush(myItemColor);
        // 描边
        item->setPen(QPen(myItemColor));
    }
}

void RadarScene::setFont(const QFont &font)
{
    myFont = font;

    if (isItemChange(DiagramTextItem::Type))
    {
        QGraphicsTextItem *item = qgraphicsitem_cast<DiagramTextItem *>(selectedItems().first());
        //At this point the selection can change so the first selected item might not be a DiagramTextItem
        if (item)
            item->setFont(myFont);
    }
}

/**
 * @brief 修改xml文件, 更新当前组件的位置，颜色，id，和callFunction以及端口
 * @param pos
 * @param item
 */
void RadarScene::modifyXmlItems(QPointF pos, DiagramItem *item)
{
    QDomElement comp;
    comp = doc.createElement(item->getIconName());

    QDomElement color = doc.createElement("color");
    QDomElement call = doc.createElement("call");
    QDomAttr posx = doc.createAttribute("pos_x");
    QDomAttr posy = doc.createAttribute("pos_y");
    QDomAttr id = doc.createAttribute("id");
    posx.setValue(QString::number(pos.x()));
    posy.setValue(QString::number(pos.y()));
    id.setValue(item->itemSuuid);
    comp.setAttributeNode(posx);
    comp.setAttributeNode(posy);
    comp.setAttributeNode(id);
    QDomText c = doc.createTextNode(itemColor().name());
    color.appendChild(c);
    comp.appendChild(color);

    QDomText ca = doc.createTextNode(item->callFunction);
    call.appendChild(ca);
    comp.appendChild(call);

    QList<ItemPort *> ps = item->ports();
    int size = ps.size();
    for (int i = 0; i < size; ++i)
    {
        QDomElement port = doc.createElement("port");
        port.appendChild(doc.createTextNode(ps[i]->getUuid()));
        port.setAttribute("name", ps[i]->portName());
        port.setAttribute("isoutput", ps[i]->isOutput() == true ? 1 : 0);
        port.setAttribute("port_type", ps[i]->portFlags());
        port.setAttribute("dataType", ps[i]->getDataType());
        comp.appendChild(port);
    }
    Items.appendChild(comp);
}
/**
 * @brief 更新元素位置信息
 * @param pos
 * @param item
 */
void RadarScene::updateXmlItemsPos(QPointF pos, DiagramItem *item)
{
    QDomNode node = doc.elementsByTagName("Items").at(0).firstChild();
    QDomElement elem;
    // 遍历items标签找到
    while(!node.isNull())
    {
        if(node.isElement())
        {
            elem = node.toElement();
            if(elem.attribute("id").compare(item->itemSuuid) == 0)
            {
                elem.setAttribute("pos_x", pos.x());
                elem.setAttribute("pos_y", pos.y());
//                MainWindow_Radar::isSave = false;
                dynamic_cast<MainWindow_Radar*>(this->parent())->isSave=false;
                //                 qDebug() << "xml由于位置改变而被修改";
                emit isSave2False(nullptr);
                return;
            }
        }
        node = node.nextSibling();
    }
    qDebug() << "[warning] 未找到匹配的id=" << item->itemSuuid;
}

/**
 * @brief 修改箭头信息
 * @param arrow
 * @param startItem
 * @param endItem
 */
void RadarScene::modifyXmlArrows(Arrow *arrow, DiagramItem *startItem, DiagramItem *endItem)
{
    QDomElement arr = doc.createElement("arrow");
    QDomElement color = doc.createElement("color");
    QDomAttr start = doc.createAttribute("start_item_id");
    QDomAttr end = doc.createAttribute("end_item_id");
    QDomAttr id = doc.createAttribute("id");
    start.setValue(startItem->itemSuuid);
    end.setValue(endItem->itemSuuid);
    id.setValue(arrow->itemId);
    arr.setAttributeNode(start);
    arr.setAttributeNode(end);
    arr.setAttributeNode(id);
    QDomText c = doc.createTextNode(myLineColor.name());
    color.appendChild(c);

    QDomElement sp = doc.createElement("start_port");
    QDomText sdt = doc.createTextNode(arrow->port1()->getUuid());
    sp.appendChild(sdt);
    QDomElement ep = doc.createElement("end_port");
    QDomText edt = doc.createTextNode(arrow->port2()->getUuid());
    ep.appendChild(edt);

    arr.appendChild(color);
    arr.appendChild(sp);
    arr.appendChild(ep);
    Arrs.appendChild(arr);
}


void RadarScene::setMode(Mode mode)
{
    myMode = mode;
}

void RadarScene::setMyItemIconName(QString value)
{
    myItemIconName = value;
}
//处理箭头删除键发出的信号
void RadarScene::deal_arrowdelte(QString ID)
{
    //qDebug() <<"信号已经传到radarscene,箭头ID为："<<ID;

    emit delete_arrowSignal(ID);

}
//void RadarScene::setItemType(DiagramItem::DiagramType type)
//{
//    myItemType = type;
//}

/**
 * @brief 文本框失去焦点
 * @param item
 */
void RadarScene::editorLostFocus(DiagramTextItem *item)
{
    //获取光标
    QTextCursor cursor = item->textCursor();
    //取消选中
    cursor.clearSelection();
    item->setTextCursor(cursor);
    //如果里面没有内容，就自动删除吧
    if (item->toPlainText().isEmpty())
    {
        removeItem(item);
        //计划删除此对象
        item->deleteLater();
    }
}


// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         显示波形
* @author        Antrn
* @date          2019-09-10
*/
void RadarScene::startRunCode()
{
    if(dynamic_cast<MainWindow_Radar*>(this->parent())->isSave == false)
    {
        //        tr("检测到场景还未保存，是否保存后执行?")
        emit isSave2False("检测到场景还未保存，是否保存后执行?");
    }
    emit startRun();
    qDebug() << "mmmmmmmmmmmm" << "这里是波形展示";
#if 0
    {
        // 先展示出配置窗口
        //    QDir dir(QDir::currentPath());
        //    dir.cdUp();
        //    dir.cd(qAppName()+"/Projects/test/code");
        //    QString workpath = dir.absolutePath();
        //    qDebug() << "workpath" << workpath;
        //    RunCompConf *run = new RunCompConf(workpath+"/result.txt",nullptr);

        QString outputDirName = nullptr;
        QString waveDataDirName = QDir::currentPath() + "/radar/" + dynamic_cast<MainWindow_Radar*>(this->parent())->getEquip_id() + "/room/waveData/";
        outputDirName = Utils::getOutputOrInputDirName(waveDataDirName, "OUTPUT");
        if(outputDirName.compare("error") == 0)
        {
            qDebug() << "出现错误，注意一下";
            Utils::alert(qApp->desktop()->rect(), "请检查是否是数据文件不存在！");
        }
        else
        {
            RunCompConf *run = new RunCompConf(waveDataDirName + outputDirName + "/data.txt", nullptr);
            run->exec();
            sendRate(100);
            emit overRun();
        }
    }
#endif
    emit overRun();
}

void RadarScene::sendRate(float rate)
{
    emit rateSignal(rate);
}

/**
 * @brief 将拖入/点击进入场景中的组件添加到scene的一个map中去，并生成工程空间文件
 * @param ap
 */
void RadarScene::receiveAlgo4listWidget(AlgorithmComp pap)
{
    this->ap = pap;
}

/**
 * @brief 当放下组件到场景空间中时候，新建文件
 */
void RadarScene::createFile2zoom(QString sid)
{
    qDebug() << "!!!!@@@@" << this->ap.getInfo();
    if(!this->ap.getInfo().isEmpty())
    {
        // 复制一份bu是原来的那个
        QMap<QString, QString> newm;
        newm.insert("ID", sid);
        newm.insert("Path", QDir::currentPath() + "/radar/" + dynamic_cast<MainWindow_Radar*>(parent())->getEquip_id() + "/room/algoXml");
        QDateTime *dt = new QDateTime;
        QString dtime = dt->currentDateTime().toString();
        newm.insert("Time", dtime);
        newm.insert("Name", ap.getInfo().take("Name"));
        newm.insert("CN_Name", ap.getInfo().take("CN_Name"));
        newm.insert("callFunction", ap.getInfo().take("callFunction"));
        ap.setInfo(newm);
        ap.setFileName(ap.getInfo().take("Name"));
        // 5.9.8
        // qDebug() << ap.getInfo().toStdMap();
        scene_comps.insert(sid, ap);
        Utils::writeAlgorithmComp2Xml(ap, "/radar/" + dynamic_cast<MainWindow_Radar*>(parent())->getEquip_id() + "/room");
        // 遍历Map打印一下
        //        foreach(const QString ac, scene_comps.keys()){
        // 5.9.8
        // qDebug() << "算法组件的id： " << ac << ", 组件信息: " << scene_comps.value(ac).getInfo().toStdMap();
        //        }
    }
    else
    {
        qDebug() << "没有点击或者拖动";
    }
}


// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-04
 * @brief RadarScene::getPara2Port 属性（算法参数）变端口
 * @param fileName 算法组件库中的xml文件
 * @param item 图元对象
 */
void RadarScene::getPara2Port(QString fileName, DiagramItem *item)
{
    // 先读取算法组件库的xml文件，将信息存入到doc中
    QDomDocument doc;
    QFile file(fileName.toUtf8());
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "算法组件" + myItemIconName + "的xml文件打开出错！";
        file.close();
    }
    if(!doc.setContent(&file))
    {
        file.close();
        qDebug() << "算法组件的xml文件读取失败, 请查看是否是中文命名";
    }
    file.close();
    //读取参数
    QDomNode ParaNode = doc.elementsByTagName("Parameter").at(0);
    int port_num = ParaNode.childNodes().size();
    for (int i = 0; i < port_num; ++i)
    {
        QDomElement para = ParaNode.childNodes().at(i).toElement();
        // 参数名
        QString tarname = para.tagName();
        // 如果是输出读端口类型
        if(para.attribute("isoutput").compare("1") == 0)
        {
            item->addOutputPort(tarname, QUuid::createUuid().toString(), para.attribute("type"));
        }
        else if(para.attribute("isoutput").compare("0") == 0)
        {
            item->addInputPort(tarname, QUuid::createUuid().toString(), para.attribute("type"));
        }
        else  //不接受或者不传输数据的 参数 不添加为端口
        {
            //  item->addPort(tarname, QUuid::createUuid().toString(), 0, 3);
        }
    }
}

#if 1

/**
 * @brief 鼠标点击场景
 * @param mouseEvent
 */
void RadarScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //必须是左键才进行处理
    if (mouseEvent->button() != Qt::LeftButton)
        return;


    if(itemAt(mouseEvent->scenePos(), QTransform()) == nullptr && !rubberBand)
    {
        isDrag = false;
        origin = mouseEvent->scenePos();
        rubberBand = new QRubberBand(QRubberBand::Line);
        rubberBand->setGeometry(QRect(origin.toPoint(), QSize()));
        rubberBand->show();
    }

    DiagramItem *item;
    switch (myMode)
    {
    case InsertItem:
    {
#if 0
        //限制只有一个输入组件
        if(myItemIconName.compare("INPUT") == 0)
        {
            if(isExistInput)
            {
                Utils::alert(qApp->desktop()->rect(), "场景中已经有输入组件!");
                myItemIconName = "";
                // 不然还是插入模式，还会插入一个空组件
                setMode(MoveItem);
                return;
            }
            else
            {
                isExistInput = true;
            }
        }
        if(myItemIconName.compare("OUTPUT") == 0)
        {
            if(isExistOutput)
            {
                Utils::alert(qApp->desktop()->rect(), "场景中已经有输出组件!");
                myItemIconName = "";
                // 不然还是插入模式，还会插入一个空组件
                setMode(MoveItem);
                return;
            }
            else
            {
                isExistOutput = true;
            }
        }
#endif
        // NOTE 这里和drop事件中的代码几乎一样，可以提取出来优化一下
        qDebug() << "此时是InsertItem模式";
        QString sid = QUuid::createUuid().toString();
        // Antrn: 新建组件
        item = new DiagramItem(sid, myItemIconName, myItemMenu, dynamic_cast<MainWindow_Radar*>(parent())->getEquip_id());
        item->setBrush(myItemColor);

        // ap是当前被添加的算法组件
        item->CN_Name = ap.getInfo().take("CN_Name"); //设置中文名称
        // Antrn: 设置函数调用
        item->callFunction = ap.getInfo().take("callFunction");
        // 加到场景中
        addItem(item);
        // 20200713 新增
        idList.append(sid);
        // 设置位置
        item->setPos(mouseEvent->scenePos());
        // 为组件添加（端口）名称和副名称
        item->addPort(myItemIconName, QUuid::createUuid().toString(), 0, ItemPort::NamePort);
        // Antrn: 中文名
        item->addPort(this->ap.getInfo()["CN_Name"], QUuid::createUuid().toString(), 0, ItemPort::TypePort);

        // Antrn: 组件库中该组件的xml位置
        QString algoPath = QDir::currentPath() + "/algoXml/" + myItemIconName + ".xml";
        // Antrn: 将属性渲染到端口上
        getPara2Port(algoPath, item);
        emit itemInserted(item);
        modifyXmlItems(mouseEvent->scenePos(), item);
        // 创建工程空间文件
        createFile2zoom(sid);
        // Antrn: 显示当前算法组件的属性，绑定信号和槽
        connect(item, &DiagramItem::showItemsProperties, dynamic_cast<MainWindow_Radar*>(this->parent()), &MainWindow_Radar::receiveItemsid2showProperties);
        //点击组件显示队列信息
        connect(item, &DiagramItem::show_comp_lineinfo, dynamic_cast<MainWindow_Radar*>(this->parent()), &MainWindow_Radar::receiveIteminformation);
        break;
    }
    case InsertLine:
    {
        qDebug() << "此时是InsertLine模式";
        QTransform transform;
        QGraphicsItem *itemI = this->itemAt(mouseEvent->scenePos(), transform);
        if (itemI && itemI->type() == ItemPort::Type)
        {
            qDebug() << "点到端口上了";
            // 更新这条线段
            conn = new Arrow();
            ItemPort *p1 = dynamic_cast<ItemPort*>(itemI);
            conn->setPort1(p1);
            // 鼠标操作在某个端口的所在的位置
            conn->setPos1(itemI->scenePos());
            // 鼠标当前在场景中的位置
            conn->setPos2(mouseEvent->scenePos());
            conn->updatePath();
            addItem(conn);
        }
        //            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
        //                                        mouseEvent->scenePos()));
        //            line->setPen(QPen(myLineColor, 2));
        //            addItem(line);
        break;
    }
    //        case InsertText:
    //            textItem = new DiagramTextItem();
    //            textItem->setFont(myFont);
    //            textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    //            textItem->setZValue(1000.0);
    //            connect(textItem, SIGNAL(lostFocus(DiagramTextItem*)),
    //                    this, SLOT(editorLostFocus(DiagramTextItem*)));
    //            connect(textItem, SIGNAL(selectedChange(QGraphicsItem*)),
    //                    this, SIGNAL(itemSelected(QGraphicsItem*)));
    //            addItem(textItem);
    //            textItem->setDefaultTextColor(myTextColor);

    //            textItem->setPos(mouseEvent->scenePos());
    //            emit textInserted(textItem);
    //            break;
    case MoveItem:
    {
//        qDebug() << "此时是MoveItem模式";
        break;
    }
    default:
        break;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void RadarScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    emit signal_xy(mouseEvent->scenePos().x(), mouseEvent->scenePos().y());
    // 如果当前已在拖动
    if (myMode == InsertLine && conn)
    {
        conn->setPos2(mouseEvent->scenePos());
        conn->updatePath();
    }
    if (myMode == MoveItem)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        update();
    }

    if(rubberBand && (mouseEvent->buttons() & Qt::LeftButton ))
    {
        isDrag = true;
//        qDebug() << "鼠标移动";
        rubberBand->setGeometry(QRect(origin.toPoint(), mouseEvent->scenePos().toPoint()).normalized());
    }
    //    if (myMode == InsertLine && line != nullptr) {
    //        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
    //        line->setLine(newLine);
    //    } else if (myMode == MoveItem) {
    //        QGraphicsScene::mouseMoveEvent(mouseEvent);
    //        update();
    //    }
}

/**
 * @brief 松开鼠标的时候，划箭头
 * @param mouseEvent 鼠标在场景中移动时的触发事件
 */
void RadarScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (conn != nullptr && mouseEvent->button() == Qt::LeftButton && myMode == InsertLine)
    {
        QTransform transform;
        // 端口
        QGraphicsItem *item = itemAt(mouseEvent->scenePos(), transform);

        if (item && item->type() == ItemPort::Type)
        {
            ItemPort *port1 = conn->port1();
            ItemPort *port2 = dynamic_cast<ItemPort*>(item);

            // 相同输入或者输出端口不能连
            if ((port1->isOutput() && !port2->isOutput()) && port1->block() != port2->block() && !port1->isConnected(port2))
            {
                qDebug() << port1->getDataType() << "---" << port2->getDataType();
                // FIXME 端口数据类型不一致提醒不能连接
                if(port2->getDataType().compare(port1->getDataType()) != 0)
                {
                    port2->setCursor(QCursor(Qt::ForbiddenCursor));
                    Utils::alert(qApp->desktop()->rect(), "数据类型不一致！");
                    qDebug() << "前" << port1->getConnections().size();
                    port1->removeConnection(conn);
                    qDebug() << "后" << port1->getConnections().size();
                    removeItem(conn);
                }
                else // Antrn: 数据类型相同
                {
                    port2->setCursor(Qt::ArrowCursor);
                    conn->setPos2(port2->scenePos());
                    conn->setPort2(port2);
                    conn->updatePath();

                    //找到线段的开始点端口p1处所属的Item
                    DiagramItem * startItem = conn->port1()->block();

                    DiagramItem * endItem = conn->port2()->block();

                    //防止点线段
                    qDebug() << "到这里" << startItem->getIconName() << endItem->getIconName();
                    // Antrn: 防止点线段
                    if (startItem && endItem &&
                            startItem->type() == DiagramItem::Type &&
                            endItem->type() == DiagramItem::Type &&
                            startItem != endItem)
                    {
                        conn->setMyStartItem(startItem);
                        conn->setMyEndItem(endItem);
                        startItem->addArrow(conn);
                        endItem->addArrow(conn);

                        Arrow *arrow = new Arrow(startItem, endItem);
                        arrow->setColor(myLineColor);
                        QString sid = QUuid::createUuid().toString();
                        arrow->itemId = sid;

                        arrow->setPort1(port1);
                        arrow->setPort2(port2);
                        arrow->setPos1(port1->scenePos());
                        arrow->setPos2(port2->scenePos());
                        startItem->addArrow(arrow);
                        endItem->addArrow(arrow);
                        idList.append(sid);

                        qDebug() << "新箭头的ID: " << sid << "; 所有id:" << idList;
                        //处理箭头的删除信号
                        connect(arrow, &Arrow::deletearrow_signal, this, &RadarScene::deal_arrowdelte);
                        addItem(arrow);
                        MainWindow_Radar *parent =  dynamic_cast<MainWindow_Radar *>(this->parent());

                        parent->isSave = false;
                        parent->toggleSaveXml(1);
                        qDebug() << "xml由于箭头添加而改变";
                        emit parent->send2AppOutput("[提示]xml由于箭头添加而改变", 1);
                        modifyXmlArrows(arrow, startItem, endItem);
                        // Antrn: 删除临时的箭头，所以每次连接箭头的时候控制台也会报删除箭头
                        startItem->removeArrow(conn);
                        endItem->removeArrow(conn);
                        port1->removeConnection(conn);
                        port2->removeConnection(conn);
                        removeItem(conn);
                        arrow->updatePath();

                        // Antrn: 如果连接到了显示组件，则弹出信号显示页面
                        if(endItem->getIconName().compare("display") == 0)
                        {
                            qDebug() << "连接到了显示组件，要订阅的组件是：" << startItem->getIconName() << "组件数据大小：" << startItem->getBufferSize();
                            // Antrn: TODO 接入显示组件的窗口，show出来
                            RealTimeSignal *signal_widget = new RealTimeSignal();
                            signal_widget->show();
                        }
                    }
                    else
                    {
                        qDebug() << "点线段了吧！检查一下！";
                    }
                    // 必须要清掉，因为鼠标按压移动的时候要用到，不清掉的话，在哪里拖动都会产生线段
                    conn = nullptr;
                }
            }
            else
            {
                Utils::alert(qApp->desktop()->rect(), "不能互相连接！");
                port2->setCursor(Qt::ForbiddenCursor);
//                qDebug() << "前" << port1->getConnections().size();
                port1->removeConnection(conn);
//                qDebug() << "后" << port1->getConnections().size();
                this->removeItem(conn);
            }
        }
        else
        {
            qDebug() << "箭头结尾没有任何对象";
//            qDebug() << "前" << conn->port1()->getConnections().size();
            conn->port1()->removeConnection(conn);
//            qDebug() << "后" << conn->port1()->getConnections().size();
            this->removeItem(conn);
        }
    }
    conn = nullptr;
    delete conn;

    if(rubberBand && isDrag)
    {
        qDebug() << "隐藏RubberBand，显示出框住（选中）的组件和箭头对象：";
        rubberBand->hide();
        // 测试选中的所有items
        QList<QGraphicsItem*> l = this->selectedItems();
        // Antrn: 分别计算组件数量和箭头数量
        int diagramNum = 0, arrowNum = 0;
        for (auto &i : l)
        {
            DiagramItem *d;
            Arrow *a;
            if( (d = dynamic_cast<DiagramItem*>(i)))
            {
//                qDebug() << d->getIconName();
                diagramNum++;
            }
            if((a = dynamic_cast<Arrow*>(i)))
            {
                arrowNum++;
//                qDebug() << a->itemId;
            }
        }

        // 如果框选的有item，可以弹窗显示组件打包窗口
        if(diagramNum > 1)
        {
//            QString tip = QString("组件个数%1, 可以打包").arg(diagramNum);
//            Utils::alert(qApp->desktop()->rect(), tip);
            // Antrn: 调用显示窗口函数
            dynamic_cast<MainWindow_Radar*>(this->parent())->libButtonTriggered();
        }
        rubberBand = nullptr;
        // Antrn: 不再拖动了
        isDrag = false;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//void RadarScene::focusInEvent(QFocusEvent *)
//{
//    qDebug() << "scene focus in";
//    // 暂时无用
//}

/**
 * @brief 场景丢失鼠标焦点
 */
void RadarScene::focusOutEvent(QFocusEvent *)
{
//    qDebug() << "scene focus out";
    if(myItemMenu->isEnabled() && isSelected)
    {
        //        myItemMenu->setEnabled(true);
        //        qDebug() << "myItemMenu设置为true" << myItemMenu->isEnabled();
        isSelected = false;
    }
    else
    {
        myItemMenu->setEnabled(false);
//        qDebug() << "myItemMenu设置为false->" << myItemMenu->isEnabled();
    }
}

/**
 * @brief 从组件列表拖入事件
 * @param event
 */
void RadarScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(RadarCompDraglistWidget::puzzleMimeType()))
        event->accept();
    else
        event->ignore();
    qDebug() << "组件被托入到场景中";
}

/**
 * @brief 将算法组件放下事件
 * @param event
 */
void RadarScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(RadarCompDraglistWidget::puzzleMimeType()))
    {
#if 0
        if(myItemIconName.compare("INPUT") == 0)
        {
            if(isExistInput)
            {
                Utils::alert(qApp->desktop()->rect(), "场景中已经有输入组件!");
                setMode(MoveItem);
                event->ignore();
                return;
            }
            else
            {
                isExistInput = true;
            }
        }
        if(myItemIconName.compare("OUTPUT") == 0)
        {
            if(isExistOutput)
            {
                Utils::alert(qApp->desktop()->rect(), "场景中已经有输出组件!");
                myItemIconName = "";
                // 不然还是插入模式，还会插入一个空组件
                setMode(MoveItem);
                return;
            }
            else
            {
                isExistOutput = true;
            }
        }
#endif
        event->acceptProposedAction();

        // 唯一id
        QString sid = QUuid::createUuid().toString();
//        qDebug() << "新生成的sid: " << sid;
//        item->itemSuuid = sid;

        // 新建组件对象
        DiagramItem *item = new DiagramItem(sid, myItemIconName, myItemMenu, dynamic_cast<MainWindow_Radar*>(parent())->getEquip_id());
        item->setBrush(myItemColor);
        item->CN_Name = ap.getInfo().take("CN_Name"); //设置中文名称
        item->callFunction = ap.getInfo().take("callFunction");
        addItem(item);
        // 把id加到scene中记录，一定要时刻保持list中的id和场景中的对应
        idList.append(sid);
        // 设置位置
        item->setPos(event->scenePos());
        // 添加组件名字端口
        item->addPort(myItemIconName, QUuid::createUuid().toString(), 0, ItemPort::NamePort);
        // 增加中文端口，斜体
        item->addPort(item->CN_Name, QUuid::createUuid().toString(), 0, ItemPort::TypePort);//item->getIconName()
        // 组件库中的算法xml文件路径
        QString algoPath = QDir::currentPath() + "/algoXml/" + myItemIconName + ".xml";
        // 将组件的参数渲染到当前图元的端口上
        getPara2Port(algoPath, item);
        // Antrn: 组件插入触发信号
        emit itemInserted(item);
        // Antrn: 修改xml，相当于保存
        modifyXmlItems(event->scenePos(), item);
        // 复制文件到/room/algoXml/
        createFile2zoom(sid);
        // Antrn: 展示组件的属性，信号槽绑定
        connect(item, &DiagramItem::showItemsProperties, dynamic_cast<MainWindow_Radar*>(this->parent()), &MainWindow_Radar::receiveItemsid2showProperties);
        //点击组件显示队列信息
        connect(item, &DiagramItem::show_comp_lineinfo, dynamic_cast<MainWindow_Radar*>(this->parent()), &MainWindow_Radar::receiveIteminformation);

        //        QByteArray comData = event->mimeData()->data(RadarCompDraglistWidget::puzzleMimeType());
        //        QDataStream dataStream(&comData, QIODevice::ReadOnly);
        //        QPixmap pixmap;
        //        QString str, id;
        //        dataStream >> pixmap >> str >> id;
        //        qDebug() << pixmap << "; " << str << "; " << id;

        event->setDropAction(Qt::MoveAction);
        event->accept();
        
    }
    else
    {
        event->ignore();
    }
}

void RadarScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(RadarCompDraglistWidget::puzzleMimeType()))
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

int RadarScene::getCompNums() const
{
    return CompNums;
}

void RadarScene::setCompNums(int value)
{
    CompNums = value;
}

#endif
//是否有选中的类型
bool RadarScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems())
    {
        if (item->type() == type)
            return true;
    }
    return false;
}

bool RadarScene::getIsExistDisplay() const
{
    return isExistDisplay;
}

void RadarScene::setIsExistDisplay(bool value)
{
    isExistDisplay = value;
}

bool RadarScene::getIsExistInput() const
{
    return isExistInput;
}

void RadarScene::setIsExistInput(bool value)
{
    isExistInput = value;
}
//设置输出组件的存在情况
void RadarScene::setIsExistOutput(bool value)
{
    isExistOutput = value;
}

QMap<QString, AlgorithmComp> RadarScene::getScene_comps() const
{
    return scene_comps;
}

void RadarScene::setScene_comps(const QMap<QString, AlgorithmComp> &value)
{
    scene_comps = value;
}

/**
 * @brief 添加组件
 * @param key 算法组件id
 * @param a 算法
 */
void RadarScene::add2Scene_comps(QString key, AlgorithmComp &a)
{
    this->scene_comps.insert(key, a);
}

/**
 * @brief 删掉Map中一个元素
 * @param id 算法组件id
 */
void RadarScene::deleteScene_comps(QString id)
{
    this->scene_comps.remove(id);
}

QColor RadarScene::getMyItemColor() const
{
    return myItemColor;
}

void RadarScene::setMyItemColor(const QColor &value)
{
    myItemColor = value;
}

QDomElement *RadarScene::getItems()
{
    return &Items;
}

void RadarScene::setItems(const QDomElement &value)
{
    Items = value;
}

QDomElement *RadarScene::getArrs()
{
    return &Arrs;
}

void RadarScene::setArrs(const QDomElement &value)
{
    Arrs = value;
}

void RadarScene::setDoc(const QDomDocument &value)
{
    doc = value;
}

QString RadarScene::getMyItemIconName() const
{
    return myItemIconName;
}
