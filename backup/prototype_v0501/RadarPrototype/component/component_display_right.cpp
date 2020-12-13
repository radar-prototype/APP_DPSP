#include "component_display_right.h"
#include <QGraphicsView>
#include <QDebug>
#include <QUuid>
#include <QMenu>
#include  "diagram_item.h"
#include "algorithmcomp.h"
#include <QGraphicsItem>
#include <QPointF>
#include <QDir>
#include <QDomDocument>
#include <QHBoxLayout>

Component_display_right::Component_display_right(QObject *parent) : QObject(parent)
{

}
void Component_display_right::displayCompenent(QWidget* parentWidget, AlgorithmComp* algorithmComp)
{


    // 不然  切换按钮不更新
    foreach (QObject * ob, parentWidget->children())
    {
        delete ob;
    }

    QGraphicsView* grahicsView = new QGraphicsView();
    grahicsView->setParent(parentWidget);
    QHBoxLayout *layout = new QHBoxLayout(parentWidget);
    layout->addWidget(grahicsView);
    layout->setMargin(0);
    parentWidget->setLayout(layout);

    //凑构造方法用的 没有用的
    QMenu  menu;
    QGraphicsScene *scene = new QGraphicsScene(parentWidget) ;
    Diagram_item *item = new Diagram_item(&menu);
    item->setParent(scene);
//     item->setBrush(myItemColor);
    QString sid = QUuid::createUuid().toString();
    qDebug() << "display新生成的sid: " << sid;
    item->itemSuuid = sid;
    item->CN_Name = algorithmComp->getInfo().take("CN_Name"); //设置中文名称
    item->callFunction = algorithmComp->getInfo().take("callFunction");
    QString file_name = algorithmComp->getFileName();

    //不会触发原来的算法组建item的鼠标事件
    item->setActive(false);
    item->setCursor(QCursor(Qt::ForbiddenCursor));
    item->setAcceptedMouseButtons(Qt::NoButton);
    item->setFlag(QGraphicsItem::ItemIsMovable, false);
    item->setFlag(QGraphicsItem::ItemIsSelectable, false);
    item->setFlag(QGraphicsItem::ItemIsFocusable, false);



    //加入组建item到scene
    scene->addItem(item);
    item->setPos( QPointF( 0, 0));
    item->addPort(file_name, QUuid::createUuid().toString(), 0, ItemPort::NamePort);
    item->addPort(item->CN_Name, QUuid::createUuid().toString(), 0, ItemPort::TypePort);
    // 组件库中的算法xml文件路径
    QString algoPath = QDir::currentPath() + "/algoXml/" + file_name + ".xml";
    // 将组件的参数渲染到图元的端口上
    getPara2Port(algoPath, item);

    grahicsView->setScene(scene);

}

/**
 *复制过来的方法
 * @brief RadarScene::getPara2Port 属性（算法参数）变端口
 * @param fileName 算法组件库中的xml文件
 * @param item 图元对象
 */
void Component_display_right::getPara2Port(QString fileName, DiagramItem *item)
{
    QDomDocument doc;
    QFile file(fileName.toUtf8());
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "算法组件" + fileName + "的xml文件打开出错！";
        file.close();
    }
    if(!doc.setContent(&file))
    {
        file.close();
        qDebug() << "算法组件的xml文件读取失败, 请查看是否是中文命名";
    }
    file.close();
    QDomNode ParaNode = doc.elementsByTagName("Parameter").at(0);
    int port_num = ParaNode.childNodes().size();
    for (int i = 0; i < port_num; ++i)
    {
        QDomElement para = ParaNode.childNodes().at(i).toElement();
        QString tarname = para.tagName();
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
