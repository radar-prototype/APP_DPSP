#include "component_info.h"
#include <QGridLayout>
#include "algorithmcomp.h"
#include "radarscene.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "utils.h"
Component_info::Component_info(QWidget *parent) : QWidget(parent)
{

}
void Component_info::setInfoWindow(QWidget *parentWidget,AlgorithmComp* ac){

    // 不然  切换按钮不更新窗口中的信息
    foreach (QObject* ob, parentWidget->children()) {   delete ob;  }

    // 获取info
    QMap<QString, QString> info_map = ac->getInfo();

    QMap<QString, QString>::Iterator it;


    parentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //下面循环使用到的几个工具类
    QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(parentWidget);
    QtStringPropertyManager *stringManager = new QtStringPropertyManager(parentWidget);
    QtStringPropertyManager *stringManagerIsReadonly = new QtStringPropertyManager(parentWidget);
    QtBoolPropertyManager *boolManager = new QtBoolPropertyManager(parentWidget);

    //info
    QtProperty *item01 = groupManager->addProperty("基本信息");
    for ( it = info_map.begin(); it != info_map.end(); ++it ) {
        qDebug()<<it.key()<<" "<<it.value();
        QtProperty *item1 ;
        //过滤 id 单独的 stringManagerIsReadonly 方便设置其他可编辑 （这里没有给予编辑功能所以不影响）
        if(QString::compare("ID",it.key())==0){
            item1 = stringManagerIsReadonly->addProperty(it.key());
            stringManagerIsReadonly->setValue(item1,it.value());
        }else{
            item1 = stringManager->addProperty(it.key());
            stringManager->setValue(item1,it.value());
        }
        item01->addSubProperty(item1);
    }
    //describe
    QtProperty *item02 = stringManager->addProperty("算法描述");
    stringManager->setValue(item02,ac->getDesc());

    //parameter  //item003是因为他里面还有嵌套就这么叫了
    QtProperty *item003 = groupManager->addProperty("算法属性");
    QtProperty *item004 = groupManager->addProperty("算法端口");

    QMap<QString, QMap<QString, QString>> para_map = ac->getParam();
    for ( QMap<QString, QMap<QString, QString>>::iterator itt = para_map.begin(); itt != para_map.end(); ++itt )
    {
        QtProperty *item03 = stringManager->addProperty(itt.key());

        QtProperty *item1 = stringManager->addProperty("备注");
        stringManager->setValue(item1,itt.value().value("describe"));

        QtProperty *item2 = stringManager->addProperty("参数类型");
        stringManager->setValue(item2,itt.value().value("type"));//参数值为数字的话，编辑的时候可以换成数字编辑框的

        QtProperty *item4 = stringManager->addProperty("参数值");
        stringManager->setValue(item4,itt.value().value("value"));//参数值为数字的话，编辑的时候可以换成数字编辑框的

        QString isoutput_sign=itt.value().value("isoutput");//"2"->属性    '1'->输出端口   ‘3’->输入端口

        item03->addSubProperty(item1);
        item03->addSubProperty(item2);
        item03->addSubProperty(item4);
        if (isoutput_sign=="2"){
            item03->addSubProperty(item4);
            item003->addSubProperty(item03);
        }
        else{
            QtProperty *item3=boolManager->addProperty("是否为输出端口");
            if (isoutput_sign=="1"){
                   boolManager->setValue(item3,true);
               }
              else  if (isoutput_sign=="0"){
                   boolManager->setValue(item3,false);
               }
             item03->addSubProperty(item3);
             item004->addSubProperty(item03);
        }
    }

    //如果不可编辑 就加上一个 空的  QtAbstractPropertyBrowser
    QtAbstractPropertyBrowser *editor1 = new QtTreePropertyBrowser();


    editor1->addProperty(item01);
    editor1->addProperty(item02);
    editor1->addProperty(item003);
    editor1->addProperty(item004);

    QGridLayout *layout = new QGridLayout(parentWidget);
    QLabel *label1 = new QLabel("算法信息");

    label1->setWordWrap(true);
    label1->setFrameShadow(QFrame::Sunken);
    label1->setFrameShape(QFrame::Panel);
    label1->setAlignment(Qt::AlignCenter);
    layout->addWidget(label1, 0, 0);
    layout->addWidget(editor1, 1, 0);
    layout->setMargin(0);

}
