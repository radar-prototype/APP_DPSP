#include "mainwindow_radar_property.h"
#include <QDebug>
mainwindow_radar_property::mainwindow_radar_property(QWidget *parent) :
    QDockWidget(parent)
{

}

/**
     * @brief 展示右边的属性Dock
     * @param ac 要解析展示的算法
     * @param isReadonly 是不是可以编辑的
     */
void mainwindow_radar_property::create(AlgorithmComp ac, bool isReadonly ,RadarScene* scene,QString Equip_id)
{
    // 获取info
    QMap<QString, QString> info_map = ac.getInfo();

    QMap<QString, QString>::Iterator it;


    // [注意] 这里不要在设计界面拖入一个ScrollArea，然后再和代码结合，我搞了一上午没解决，blgl //shao:????
    //        dockWidgetContents_2 是属性窗口内部的widgetA
    //        QScrollArea *scroll = new QScrollArea(dockWidgetContents_2);

    QWidget *w = new QWidget;
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    //        scroll->setWidgetResizable(true);  // 自动调整大小

    //下面循环使用到的几个工具类
    QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(w);
    QtStringPropertyManager *stringManager = new QtStringPropertyManager(w);
    QtStringPropertyManager *stringManagerIsReadonly = new QtStringPropertyManager(w);
    QtBoolPropertyManager *boolManager = new QtBoolPropertyManager(w);
    //info
    QtProperty *item01 = groupManager->addProperty("基本信息");
    for ( it = info_map.begin(); it != info_map.end(); ++it ) {
        qDebug()<<it.key()<<" "<<it.value();
        QtProperty *item1 ;
        //过滤id 单独的 stringManagerIsReadonly 方便设置其他可编辑
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
    stringManager->setValue(item02,ac.getDesc());

    //parameter  //item003是因为他里面还有嵌套就这么叫了
    QtProperty *item003 = groupManager->addProperty("算法参数");

    QMap<QString, QMap<QString, QString>> para_map = ac.getParam();
    for ( QMap<QString, QMap<QString, QString>>::iterator itt = para_map.begin(); itt != para_map.end(); ++itt ) {
        QtProperty *item03 = stringManager->addProperty(itt.key());

        QtProperty *item1 = stringManager->addProperty("备注");
        stringManager->setValue(item1,itt.value().value("describe"));

        QtProperty *item2 = stringManager->addProperty("参数类型");
        stringManager->setValue(item2,itt.value().value("type"));//参数值为数字的话，编辑的时候可以换成数字编辑框的

        QtProperty *item4 = stringManager->addProperty("参数值");
        stringManager->setValue(item4,itt.value().value("value"));//参数值为数字的话，编辑的时候可以换成数字编辑框的

        QtProperty *item3= boolManager->addProperty("是否为输出端口");
        bool isoutput = itt.value().value("isoutput")=="1" ? true : false;
        boolManager->setValue(item3,isoutput);

        item03->addSubProperty(item1);
        item03->addSubProperty(item2);
        item03->addSubProperty(item3);
        item03->addSubProperty(item4);
        item003->addSubProperty(item03);
    }

    //如果不可编辑 就加上一个 空的  QtAbstractPropertyBrowser
    QtAbstractPropertyBrowser *editor1 = new QtTreePropertyBrowser();
    //如果可以编辑
    if(!isReadonly){
        //关系是否可以编辑
        QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(w);
        editor1->setFactoryForManager(boolManager, checkBoxFactory);
        QtLineEditFactory *lineEditFactory = new QtLineEditFactory(w);
        editor1->setFactoryForManager(stringManager, lineEditFactory);
    }


    editor1->addProperty(item01);
    editor1->addProperty(item02);
    editor1->addProperty(item003);

    QGridLayout *layout = new QGridLayout(w);
    QLabel *label1 = new QLabel("算法属性：现在不能修改");

    label1->setWordWrap(true);
    label1->setFrameShadow(QFrame::Sunken);
    label1->setFrameShape(QFrame::Panel);
    label1->setAlignment(Qt::AlignCenter);
    layout->addWidget(label1, 0, 0);
    layout->addWidget(editor1, 1, 0);

    //                提交和检查
    if(!isReadonly){
        QPushButton *Ok = new QPushButton("确 定");
        QPushButton *Cancel = new QPushButton("取 消"); // 点击取消不用有反应

        QWidget *button = new QWidget();
        QFormLayout *fl = new QFormLayout(button);
        fl->addRow(Cancel, Ok);
        layout->addWidget(button, 2, 0);
        label1->setText("场景中算法组件参数修改，点击下面的'确定'按钮提交");
        connect(Cancel, &QPushButton::clicked, [=](){ create(ac, false ,scene,Equip_id);});
        AlgorithmComp *ac2 = new AlgorithmComp;
        // 按下ok的时候，将上述修改后的属性进行更新，更新算法Algorithm，加入到scene的map中，并新建和重写文件
        connect(Ok, &QPushButton::clicked, [=](){

            QList<QtProperty*>  properties= editor1->properties();
            // 保存组件info ac.getInfo();
            QMap<QString, QString> info; // 用来存放组件简介的map
            //                ac.getDesc() 保存描述信息
            QString descript;
            //                ac.getParam();
            QMap<QString, QMap<QString, QString> >pmap;  //存放一条参数的map
            QMap<QString, QString> m; // 存放参数具体内容的map
            //将属性窗口组回组件
            {
                foreach (QtProperty* propertie, properties)  {
                    QString  name  =  propertie->propertyName();
                    // qDebug() << name;
                    if(QString::compare("基本信息",name)==0){
                        foreach (QtProperty* propertie1, propertie->subProperties()) {
                            QString  name1  =  propertie1->propertyName();
                            // qDebug() << name1;
                            info.insert(name1, propertie1->valueText());
                            if(name1.compare("Name") == 0){
                                ac2->setFileName(propertie1->valueText());
                            }
                        }
                    }else if(QString::compare("算法描述",name )==0){
                        descript=propertie->valueText();
                    } else if(QString::compare("算法参数",name )==0){
                        //                        装算法参数的list
                        foreach (QtProperty* propertie1, propertie->subProperties()) {
                            //                            装参数属性的list
                            m.clear();
                            foreach (QtProperty* propertie11, propertie1->subProperties()) {
                                QString  name11  =  propertie11->propertyName();
                                if(QString::compare("备注",name11 )==0){
                                    m.insert("describe", propertie11->valueText());
                                } else if(QString::compare("参数值", name11 )==0){
                                    m.insert("value", propertie11->valueText());
                                } else if(QString::compare("是否为输出端口",name11 )==0){
                                    m.insert("isoutput", QString::compare("True", propertie11->valueText())==0?"1":"0");
                                } else if(QString::compare("参数类型", name11 )==0){
                                    m.insert("type", propertie11->valueText());
                                }
                            }
                            pmap.insert(propertie1->propertyName(),m);
                        }
                    }
                }
            }
            //判断  参数合法性 先空着 （考虑  统一返回信息  统一警告 弹窗显示）
            {

            }
            //                装组件
            ac2->setDesc(descript);
            ac2->setInfo(info);
            ac2->setParam(pmap);

            //通知父组件更新
            //                emit compUpdata(*ac2);  //下面的代码再父组件槽函数里更好 没实现 先这样了
            // 先把scene的map中原来的一项删除
            scene->deleteScene_comps(ac2->getInfo()["ID"]);
            // 重新插入新的算法，id不变
            scene->add2Scene_comps(ac2->getInfo()["ID"], *ac2);
            // 重新写入文件
            Utils::writeAlgorithmComp2Xml(*ac2, "/radar/"+Equip_id+"/room");
            // 刷新一下属性Dock，默认不能编辑
            create(*ac2, true , scene, Equip_id);
        });

    }
    // 按下ok的时候，将上述修改后的属性进行更新，更新算法Algorithm，加入到scene的map中，并新建和重写文件

    //        w->setLayout(fl);
    //        scroll->setWidget(w);
    this->setWidget(w);
}
//        检查更新数据
mainwindow_radar_property::~mainwindow_radar_property()
{

}
