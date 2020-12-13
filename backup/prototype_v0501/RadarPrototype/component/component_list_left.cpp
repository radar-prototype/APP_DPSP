#include "component_list_left.h"
#include <QDir>
#include <QDebug>
#include <QListWidget>
#include <QWidget>
#include <QHBoxLayout>
#include "../Tool/utils.h"

Component_list_left::Component_list_left(QWidget *parent) : QWidget(parent)
{

}
//加载已有组建
void Component_list_left::loadAllComps(QWidget *parentWidget)
{

    // 新建listWidget
    this->newListWidget(parentWidget);

    //设置父组件布局样式（不然 listWidget不能填满父组件的垂直空间）
    QGridLayout *layout = new QGridLayout(parentWidget);
    layout->setMargin(0);
    parentWidget->setLayout(layout);

    QLabel* label1 = new QLabel("算法组件列表", parentWidget);
    label1->setWordWrap(true);
    label1->setFrameShadow(QFrame::Sunken);//凹陷
    label1->setFrameShape(QFrame::Panel);//凹陷
    label1->setAlignment(Qt::AlignCenter);//居中

    layout->addWidget(label1, 0, 0);
    layout->addWidget(listWidget, 1, 0);

    // 读取文件夹下所有的文件
    QString m_sProjectPath = QDir::currentPath() + "/algoXml/"; /*文件夹全路径名*/
//    qDebug() << "存本地组件库的路径:" << m_sProjectPath;
    QDir dir = Utils::openDirOrCreate(m_sProjectPath);
    dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
    QStringList filterList;
    filterList << "*.xml";
    // for循环读取有点慢
    foreach(QFileInfo fileInfo, dir.entryInfoList(filterList))
    {
        QString absolute_file_path = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.baseName();/*获取文件后名(不带后缀的文件名)*/
        if(!nameList.contains(fileName))
        {
            nameList.append(fileName);
            qDebug() << "component_list_left加入组件名:" << fileName;
        }
        else
        {
            qDebug() << "加载组件时有重复的文件名存在，文件名： " << fileName;

//            emit send2AppOutput(QStringLiteral("[警告]： 加载组件时有重复的文件名存在，文件名： ") + fileName, 0);
        }

        QListWidgetItem* item0 = new QListWidgetItem();
        *item0 = set_item(fileName);
        this->addDragItem(item0);
    }
    //item点击信号传递（目标现实算法组建信息 在右边窗口中）
    connect(listWidget, &QListWidget::itemClicked, this, &Component_list_left::clickItem);
    // 默认选择第一个
    if(listWidget->count() != 0) listWidget->item(0)->setSelected(true);
}

QListWidgetItem Component_list_left::set_item(QString fileName)
{
    QString m_sProjectPath = QDir::currentPath() + "/algoXml/";
    AlgorithmComp ac = Utils::readPluginXmlFile(m_sProjectPath + fileName + ".xml");
//        解析一个记录一个
    this->algorithms.insert(ac.getInfo()["ID"], ac);
    QListWidgetItem item0;
//        统一的图标
    item0.setIcon(QIcon(":/img/component.png"));
    item0.setText(tr(fileName.toUtf8().data()));

//  存对应的算法组件的指针 用到神奇的QVariant   不能传局部变量的指针 不然取得时候早就释放了 取不到
    item0.setData(Qt::UserRole, QVariant::fromValue((void*) &algorithms));
    //这个id不再是数字，而是uuid 从algorithms中获取组件要用
    item0.setData(Qt::UserRole + 2, ac.getInfo()["ID"]);
    item0.setData(Qt::UserRole + 1, fileName);
    return item0;
}

void Component_list_left::addDragItem(QListWidgetItem* item)
{
    listWidget->addItem(item);
    listWidget->setVisible(true);
}

void Component_list_left::newListWidget(QWidget *parentWidget)
{
    QListWidget *listWidget1 = new QListWidget(parentWidget);
    listWidget = listWidget1;
}

//为了默认显示第一个组件的属性
//QListWidgetItem* component_list_left::getWidget(){
//    if(listWidget->count()!=0)  return listWidget->item(0);
//    return nullptr;
//}

void Component_list_left:: update_component(QString name_component)
{
    if(!this->nameList.contains(name_component))
    {
        nameList.append(name_component);
        qDebug() << "是一个新建的组件，加入到列表中，组件名:" << name_component;
    }
    else
    {
        qDebug() << "是一个修改的组件 需要被更新，组件名： " << name_component;
        QListWidgetItem* item = this->listWidget->findItems(name_component, Qt::MatchExactly).first();
//        this->listWidget->removeItemWidget(item);//没用 还是delete管用
        delete item;
        listWidget->setVisible(true);
    }
    QListWidgetItem* item0 = new QListWidgetItem();
    //读文件新建item
    *item0 = set_item(name_component);
    this->addDragItem(item0);
    this->clickItem(item0);
    item0->setSelected(true);
}

QList<QString> Component_list_left::getNameList()
{
    return this->nameList;
}
QMap<QString, AlgorithmComp>* Component_list_left::get_algorithms()
{
    return &this->algorithms;
}
QListWidget* Component_list_left::get_listWidget()
{
    return this->listWidget;
}
QList<QString>* Component_list_left::get_nameList()
{
    return &this->nameList;
}
