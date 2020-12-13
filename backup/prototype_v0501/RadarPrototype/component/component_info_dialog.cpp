#include "component_info_dialog.h"
#include "ui_component_info_dialog.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QDateTime>
#include <QMessageBox>
#include "radarscene.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "utils.h"
#include "component_utile.h"
#include <QFileDialog>
Component_info_dialog::Component_info_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Component_info_dialog)
{
    ui->setupUi(this);

    //初始化
    this->stringManager = new QtStringPropertyManager(this);
    this->intManager = new QtIntPropertyManager(this);
    this->boolManager = new QtBoolPropertyManager(this);
    this->enumManager = new QtEnumPropertyManager(this);
    this->groupManager = new QtGroupPropertyManager(this);

    this->checkBoxFactory = new QtCheckBoxFactory(this);
    this->lineEditFactory = new QtLineEditFactory(this);
    this->comboBoxFactory = new QtEnumEditorFactory(this); //属性
    this->spinBoxFactory = new QtSpinBoxFactory(this);

    connect(enumManager, &QtEnumPropertyManager::valueChanged, this, [this](QtProperty * property, int val)
    {
//         qDebug()<<"enumManager"<<val<<"切换信号触发 sign"<<property->valueText()<<property->propertyName()<<property->propertyId();
        this->update_right(property, val);
    });
    connect(intManager, &QtIntPropertyManager::valueChanged, this, [this](QtProperty * property, int val)
    {
        qDebug() << "enumManager" << val << "切换信号触发 sign" << property->valueText() << property->propertyName() << property->propertyId();
        this->update_right_item5(property, val);
        QMap<QString, QMap<QString, QString> >pmapthis = this->ac.getParam();
        QMap<QString, QString> m;
        m.insert("describe", "数组长度");
        m.insert("type", "int");
        m.insert("isoutput", "2");
        m.insert("value", QString::number(val));

        pmapthis["num_filter"] = m;
        this->ac.setParam(pmapthis);
    });
}

Component_info_dialog::~Component_info_dialog()
{
    delete ui;
}

void Component_info_dialog::add_dialog(AlgorithmComp* ac)
{
    this->temp_property = ac->getParam();
    this->if_new_component=true;
//    基本信息窗口
    this->set_info_page(true);
//    属性列表加载
    set_right_info(true, "", true, false);
    set_right_info(true, "", false, false);
    this->show();
//   属性列表选择
    connect(ui->listWidget, &QListWidget::itemClicked, this, [&](QListWidgetItem * item)
    {
        set_right_info(false, item->text(), true, false);
    });
//   端口列表选择
    connect(ui->listWidget_2, &QListWidget::itemClicked, this, [&](QListWidgetItem * item)
    {
        set_right_info(false, item->text(), false, false);
    });
}

void Component_info_dialog::edit_dialog(AlgorithmComp* ac)
{
    this->ac = *ac;
    this->temp_property = ac->getParam();
//    基本信息窗口
    this->set_info_page(false);
//    属性列表加载
    this-> set_left_list();
    this->show();
//   属性列表选择
    connect(ui->listWidget, &QListWidget::itemClicked, this, [&](QListWidgetItem * item)
    {
        set_right_info(false, item->text(), true, false);
    });
//   端口列表选择
    connect(ui->listWidget_2, &QListWidget::itemClicked, this, [&](QListWidgetItem * item)
    {
        set_right_info(false, item->text(), false, false);
    });
//  默认第一个选中
    {
        if (ui->listWidget->count() != 0)
        {
            ui->listWidget->setCurrentRow(0);
            QListWidgetItem* item0 = ui->listWidget->item(0);
            emit ui->listWidget->itemClicked(item0);
        }
        else
        {
//        说明需要新建 加载一个空的信息列表
            set_right_info(true, "", true, false);
        }
        if (ui->listWidget_2->count() != 0)
        {
            ui->listWidget_2->setCurrentRow(0);
            QListWidgetItem* item0 = ui->listWidget_2->item(0);
            emit ui->listWidget_2->itemClicked(item0);
        }
        else
        {
//        说明需要新建
            set_right_info(true, "", false, false);
        }
    }
}

void Component_info_dialog::set_info_page(bool is_new_comp)
{
    if( !is_new_comp)
    {
        QString id = this->ac.getInfo()["ID"];
        QMap<QString, QString>  info = this->ac.getInfo();
        this->ui->lineEdit_sign->setText(id);
        // FIXME shao 时间读取异常 不是中文 和tostring保存的问题，因为原版的组件修改没有问题
        this->ui->dateTimeEdit->setDateTime(QDateTime::fromString(QString(info["Time"]), "M/d/yyyy h:mm AP"));
        this->ui->lineEdit_name->setText(info["Name"]);
        this->ui->lineEdit_c_name->setText(info["CN_Name"]);//获取中文名
        this->ui->lineEdit_function->setText(info["callFunction"]);//获取函数调用
        this->ui->pushButton_Path->setText(info["Path"]);
        this->ui->textEdit_introduction->setText(this->ac.getDesc());
    }
    else   //新建组件
    {
        AlgorithmComp* ac1 = new AlgorithmComp();
        this->ac = *ac1;
        this->ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
        QString id = QUuid::createUuid().toString();
        qDebug() << "组件的id为空，生成新的id： " << id;
        this->ui->lineEdit_sign->setText(id);
    }
    this->ui->lineEdit_sign->setReadOnly(true);

}

void Component_info_dialog::save_info_page()
{
    this->ac.setFileName(ui->lineEdit_name->text());
    this->ac.setDesc(ui->textEdit_introduction->toPlainText());
    QMap<QString, QString>  info;
    info.insert("Name", ui->lineEdit_name->text());
    info.insert("ID", this->ui->lineEdit_sign->text());
    info.insert("CN_Name", this->ui->lineEdit_c_name->text()); //获取中文名
    info.insert("callFunction", this->ui->lineEdit_function->text()); //函数名
//    this->ui->lineEdit_Fun->setText(ac->getInfo()["callFunction"]);//获取函数调用
    //目录用来干嘛？？管理代码相关的文件？
    if(QString::compare("点击选择路径组件文件", ui->pushButton_Path->text()) != 0)
        info.insert("Path", ui->pushButton_Path->text());
    else  info.insert("Path", "");
//     QLocale locale = QLocale::English;
    info.insert("Time", this->ui->dateTimeEdit->dateTime().toString());
//     info.insert("Time",this->ui->dateTimeEdit->dateTime().toString());
    this->ac.setInfo(info);
}
//更新左边的列表  其实应该分开端口属性分别更新的  但是我懒
void Component_info_dialog::set_left_list()
{
    ui->listWidget->clear();
    ui->listWidget_2->clear();

    QMap<QString, QMap<QString, QString>> paramList = this->ac.getParam();
    QMap<QString, QMap<QString, QString>>::iterator iter = paramList.begin();//迭代器
    while (iter != paramList.end())
    {
        if(iter.value()["isoutput"] == "2")   //是属性
        {
            if( QString::compare(iter.key(), "num_filter") != 0) //过滤掉数组长度这个属性
            {
                QListWidgetItem* item = new QListWidgetItem(tr(iter.key().toUtf8()));
                ui->listWidget->addItem(item);
            }
        }
        else  //是端口
        {
//                QListWidgetItem* item = new QListWidgetItem(tr(iter.key().toUtf8()), ui->listWidget);
            QListWidgetItem* item = new QListWidgetItem(tr(iter.key().toUtf8()));
            ui->listWidget_2->addItem(item);
        }
        iter++;
    }
}
//shao:设置属性和端口右边的输入框
/**
 * @brief Component_info_dialog::set_right_info
 * @param is_new_comp 是不是新属性 新属性 name为“”（空字符串）
 * @param name  属性name
 * @param is_property 是不是属性 区分 属性端口
 * @param isReadonly 是不是只读
 */
void Component_info_dialog::set_right_info(bool is_new_comp, QString name, bool is_property, bool isReadonly)
{
    //shao:为了端口和属性区分来
    QWidget* w_right;
    QListWidget* w_left;
    QtProperty* item0;
    QtProperty* item1;
    QtProperty* item2;
    QtProperty* item3;
    QtProperty* item4;
    QtProperty* item5;
    if(is_property)
    {
        w_right = ui->widget_edit_2;
        w_left = ui->listWidget;
    }
    else
    {
        w_right = ui->widget_p_edit;
        w_left = ui->listWidget_2;
    }

    this->temp_property = this->ac.getParam(); //原来temp_property用处很大，后来修改更新方法了，现在比较鸡肋
    QMap<QString, QString> valuelist;
    //存在 说明不是新的
    if(temp_property.contains(name))
    {
        valuelist = this->temp_property[name];
    }
    item0 = stringManager->addProperty("名称");
    item1 = stringManager->addProperty("备注");
    item2 = enumManager->addProperty("参数类型");

    if(is_property)item2->setPropertyId("property");
    else item2->setPropertyId("port");

    //shao:设置参数类型选择框
    QStringList data_type;
    data_type << "int" << "char*" << "char**";
//    this->enumManager->setEnumNames(item2, data_type);
    bool oldState = enumManager->blockSignals(true);
    this->enumManager->setEnumNames(item2, data_type);

    QMap<int, QIcon> enumIcons;
//    enumIcons[0] = QIcon(":/resource/img/1.png");
//    enumIcons[1] = QIcon(":/resource/img/2.png");
//    enumIcons[2] = QIcon(":/resource/img/3.png");
    this-> enumManager->setEnumIcons(item2, enumIcons);

    if( QString::compare(name, "") == 0)
    {
        this->stringManager->setValue(item0, "newName"); //新组件 name为默认值 newName
    }
    else
    {
        this->stringManager->setValue(item0, name); //name
    }
    if( valuelist.contains("describe"))
    {
        this->stringManager->setValue(item1, valuelist.value("describe"));
    }
    else
    {
        this->stringManager->setValue(item1, ""); //新组件 descride 默认为空
    }
    if( valuelist.contains("type"))
    {
//        enumManager->setValue(item2,2);
        for(int i = 0 ; i < 3; i++) // 解析type的值
        {
            if(QString::compare(data_type[i], valuelist.value("type")) == 0)
            {
                this->enumManager->setValue(item2, i);
                break;
            }
            else
            {
                this->enumManager->setValue(item2, 1);
            }
        }
    }
    else
    {
        this->enumManager->setValue(item2, 1); //默认为 char*
    }
    //shao:开始设置参数输入框
    int val = this->enumManager->value(item2);
    //shao:根据类型设置默认参数输入框


    switch (val)
    {
    case 0:
    {
        //int
        item4 = this->intManager->addProperty("参数值");
        this->intManager->setRange(item4, 0, 2147483647);//实际应该是无符号整数，即：0~4294967295，但是这个类用的int 没办法了
        bool oldState = intManager->blockSignals(true);
        if(valuelist.contains("value"))
        {
            int v = valuelist.value("value").toInt();
            this->intManager->setValue(item4, v);
        }
        else   //不设置值，默认为0
        {
            intManager->setValue(item4, 0);
        }
        this->intManager-> blockSignals(oldState);
        break;
    }
    case 1:
    {
        //char*
        item4 = this->stringManager->addProperty("参数值");
        if(valuelist.contains("value"))
        {
            this->stringManager->setValue(item4, valuelist.value("value"));
        }
        else
        {
            this->stringManager->setValue(item4, "");

        }
        break;
    }
    case 2:
    {
        //char**
        item4 = this->groupManager->addProperty("参数值");
        item5 = intManager->addProperty("数组长度");

        if(is_property)item5->setPropertyId("property1");
        else item5->setPropertyId("port1");

        bool oldState = intManager->blockSignals(true);

        if(temp_property.contains("num_filter"))  //取出数组长度
        {
            int value = temp_property.value("num_filter").value("value").toInt();
            if(value <= 100 and value >= 0)
                intManager->setValue(item5, value);
            else//范围超限 失败  设置为 1 的长度
                intManager->setValue(item5, 2);
        }
        else   //shao:设置新组建默认值 默认数组长度为 2
        {
            intManager->setValue(item5, 2);
        }
        item4->addSubProperty(item5);
        intManager->setRange(item5, 0, 100);

        this->intManager-> blockSignals(oldState);

        //shao: 设置数组的值
        int count = item5->valueText().toInt();
        QStringList list;
        if( valuelist.contains("value"))
        {
            list = valuelist.value("value").split(","); //值的保存方式为 value1,value2,value3...
            for(int i = 0; i < count - list.length(); i++)
            {
                list.append("");   //shao:数组不够长的话补上“”
            }
        }
        else
        {
            for(int i = 0; i < count; i++)
            {
                list.append("");   //shao:全部补上“”
            }
        }
        for(int i = 0; i < count; i++)
        {
            QtProperty *item = stringManager->addProperty("参数值" + QString(i));
            stringManager->setValue(item, list[i]);
            item4->addSubProperty(item);
        }
        break;
    }
    }
    this->enumManager-> blockSignals(oldState);
    if (!is_property)
    {
        QString isoutput_sign = valuelist.value("isoutput"); //"2"->属性    '1'->输出端口   ‘3’->输入端口
        item3 = boolManager->addProperty("是否为输出端口");
        if (isoutput_sign == "1" || is_new_comp)
        {
            boolManager->setValue(item3, true);
        }
        else  if (isoutput_sign == "0")
        {
            boolManager->setValue(item3, false);
        }
//        editor1->addProperty(item3);
    }
    if(is_property)
    {
        this->item0_property = item0;
        this->item1_property = item1;
        this->item2_property = item2;
//        this->item3_property=item3;
        this->item4_property = item4;
        this->item5_property = item5;
    }
    else
    {
        this->item0_port = item0;
        this->item1_port = item1;
        this->item2_port = item2;
        this->item3_port = item3;
        this->item4_port = item4;
        this->item5_port = item5;
    }
    this->display_right(w_right, isReadonly, is_property);
}

//更新item
void Component_info_dialog::update_right( QtProperty* property, int val)
{
    bool is_property = false;
    if(QString::compare(property->propertyId(), "property") == 0)
    {
        is_property = true;
    }
    QtProperty* item4;
    QtProperty* item5;
    switch (val)
    {
    case 0:
    {
        //int
        item4 = this->intManager->addProperty("参数值");
        this->intManager->setRange(item4, 0, 2147483647);//实际应该是无符号整数，取值是 2^31 - 1，即：0~4294967295，但是这个类用的int 没办法了
        break;
    }
    case 1:
    {
        //char*
        item4 = this->stringManager->addProperty("参数值");
        this->stringManager->setValue(item4, "");
        break;
    }
    case 2:
    {
        //char**
        item4 = this->groupManager->addProperty("参数值");
        item5 = intManager->addProperty("数组长度");
        if(is_property)item5->setPropertyId("property1");
        else item5->setPropertyId("port1");
        bool oldState = intManager->blockSignals(true);
        intManager->setValue(item5, 2);
//        }
        item4->addSubProperty(item5);
        intManager->setRange(item5, 0, 100);
        this->intManager-> blockSignals(oldState);
        //shao: 设置数组的值
        int count = item5->valueText().toInt();
        QStringList list;
        for(int i = 0; i < count; i++)
        {
            list.append("");   //shao:全部补上“”
        }
        for(int i = 0; i < count; i++)
        {
            QtProperty *item = stringManager->addProperty("参数值" + QString::number(i));
            stringManager->setValue(item, list[i]);
            item4->addSubProperty(item);
        }
        break;
    }
    }
    QWidget* w_right;
    if(is_property)
    {
        this->item4_property = item4;
        if(val == 2)this->item5_property = item5;
        w_right = ui->widget_edit_2;

    }
    else
    {
        this->item4_port = item4;
        if(val == 2) this->item5_port = item5;
        w_right = ui->widget_p_edit;
    }
    this->display_right(w_right, false, is_property);
}

void Component_info_dialog::update_right_item5(QtProperty *property, int count)
{
    //shao:非长度选着框不更新
    if(!QString::compare(property->propertyId(), "property1") == 0 && !QString::compare(property->propertyId(), "port1") == 0)
    {
        return;
    }
    bool is_property = false;
    QtProperty* item4;
    QtProperty* item5;
    QWidget* w_right;
    if(QString::compare(property->propertyId(), "property1") == 0)
    {
        is_property = true;
    }
    if(is_property)
    {
        item4 = this->item4_property;
        item5 = this->item5_property;
        w_right = ui->widget_edit_2;
    }
    else
    {
        item4 = this->item4_port;
        item5 = this->item5_port;
        w_right = ui->widget_p_edit;
    }
    QStringList list;
    QList<QtProperty*>sub_iist = item4->subProperties();
    for(int i = 1 ; i < sub_iist.length(); i++)
    {
        list.append(sub_iist.at(i)->valueText());
    }
    for(int i = sub_iist.length() - 1; i < count; i++)
    {
        list.append("");   //shao:全部补上“”
    }
    item4 = this->groupManager->addProperty("参数值");

    bool oldState = intManager->blockSignals(true);

    item4->addSubProperty(item5);

    this->intManager-> blockSignals(oldState);

    for(int i = 0; i < count; i++)
    {
        QtProperty *item = stringManager->addProperty("参数值" + QString::number(i));
        stringManager->setValue(item, list[i]);
        item4->addSubProperty(item);
    }
    if(is_property)
    {
        this->item4_property = item4;
    }
    else
    {
        this->item4_port = item4;
    }
    this->display_right(w_right, false, is_property);
}
//将 item_list里面的item显示出来
void Component_info_dialog::display_right(QWidget *w_right, bool isReadonly, bool is_property)
{
    QList<QtAbstractPropertyBrowser*> list = w_right->findChildren< QtAbstractPropertyBrowser* >();
    if (list.length() > 1) delete list.first(); //删除上一个 editor
    QtTreePropertyBrowser* editor = new QtTreePropertyBrowser();
    editor->setParent(w_right);
    if(!isReadonly)
    {
        editor->setFactoryForManager(boolManager, checkBoxFactory);
        editor->setFactoryForManager(stringManager, lineEditFactory);
        //shao:这部分有问题，从这里改起
        editor->setFactoryForManager(enumManager, comboBoxFactory);
        editor->setFactoryForManager(intManager, spinBoxFactory);
    }
    if(is_property)
    {
        editor->addProperty(this->item0_property);
        editor->addProperty(this->item1_property);
        editor->addProperty(this->item2_property);
        editor->addProperty(this->item4_property);
    }
    else
    {
        editor->addProperty(this->item0_port);
        editor->addProperty(this->item1_port);
        editor->addProperty(this->item3_port);
        editor->addProperty(this->item2_port);
        editor->addProperty(this->item4_port);
    }
    // 不然  切换按钮不更新窗口中的信息
    delete w_right->layout();
    QGridLayout *layout = new QGridLayout();
//      QLabel *label1 = new QLabel("算法属性：现在不能修改");
//      label1->setWordWrap(true);
//      label1->setFrameShadow(QFrame::Sunken);
//      label1->setFrameShape(QFrame::Panel);
//      label1->setAlignment(Qt::AlignCenter);
//      layout->addWidget(label1, 0, 0);
    layout->addWidget(editor, 1, 0);
    layout->setMargin(0);
    w_right->setLayout(layout);
}


void Component_info_dialog::pushButton_save(bool is_property)
{
    //shao:为了区分端口还是属性
    QWidget* w_right;
    QListWidget* w_left;
    if(is_property)
    {
        w_right = ui->widget_edit_2;
        w_left = ui->listWidget;
    }
    else
    {
        w_right = ui->widget_p_edit;
        w_left = ui->listWidget_2;
    }
    QList<QtAbstractPropertyBrowser*> list = w_right->findChildren< QtAbstractPropertyBrowser* >();
    QtAbstractPropertyBrowser* editor1 =  list.last();
    QList<QtProperty*>  properties = editor1->properties();
    QMap<QString, QString> m; // 存放参数具体内容的map
    //shao:临时变量
    QString propertie_name;

    QString type_sign;
    QtProperty* propertie_value;
    foreach (QtProperty* propertie, properties)
    {
        QString  name  =  propertie->propertyName();
        qDebug() << name;
        if(QString::compare("名称", name) == 0)
        {
            if(propertie->valueText().isEmpty())
            {
                //还需要后续加上其他的限制判定
                Utils::alert(qApp->desktop()->rect(), "名称不能为空");
                //更新窗口显示
                this->set_right_info(false, "", is_property, false);
                return;
            }

            propertie_name = propertie->valueText();
            // shao:首字母不能为数字 唉  都是坑
            QByteArray ba = propertie_name.toLatin1();//QString 转换为 char*
            if(ba[0] >= '0' && ba[0] <= '9')
            {
                Utils::alert(qApp->desktop()->rect(), "属性名称不能数字开头");
                return;
            }
            //shao:避免添加数组长度报错
            if(QString::compare("num_filter", propertie->valueText()) == 0)
            {
                //还需要后续加上其他的限制判定
                Utils::alert(qApp->desktop()->rect(), "数组长度属性，软件默认添加~-~");
                //更新窗口显示
                this->set_right_info(false, "", is_property, false);
                return;
            }
        }
        else if(QString::compare("备注", name ) == 0)
        {
            m.insert("describe", propertie->valueText());
        }
        else if(QString::compare("参数类型", name ) == 0)
        {
            m.insert("type", propertie->valueText());
//            qDebug()<<"111111111111"<<propertie->valueText()<<propertie->valueText().toInt();
            type_sign = propertie->valueText();
        }
        else if(QString::compare("参数值", name) == 0)
        {

            propertie_value = propertie;
//            m.insert("value", propertie->valueText());
        }
        else if(QString::compare("是否为输出端口", name) == 0)
        {
            m.insert("isoutput",  QString::compare("True", propertie->valueText()) == 0 ? "1" : "0");
        }
    }
    if(is_property)  m.insert("isoutput", "2");

    if(QString::compare(type_sign, "int") == 0)
    {
        m.insert("value", propertie_value->valueText());
    }
    else if(QString::compare(type_sign, "char*") == 0)
    {
        m.insert("value", propertie_value->valueText());
    }
    else if(QString::compare(type_sign, "char**") == 0)
    {
        QList<QtProperty*> propertie_value_list = propertie_value-> subProperties();
        QList<QString> value_list;
        foreach (QtProperty* propertie, propertie_value_list)
        {
            QString  name  =  propertie->propertyName();
            //          qDebug() << name;
            if(QString::compare("数组长度", name) == 0)
            {
//                m.insert("num_filter", propertie->valueText());
                qDebug() << propertie->valueText();
            }
            else
            {
                value_list.append(propertie->valueText());
            }
        }
        m.insert("value", value_list.join(","));
    }
    else
    {
//        不是这三种类型怎么办
    }
    if (this->temp_property.contains(propertie_name)) //已经存在  所以是修改
    {
        //判断时候已经有同名的端口或属性
        if(QString::compare(m.value("isoutput"), this->temp_property.value(propertie_name)["isoutput"]) != 0)
        {
            QMessageBox::critical(nullptr, "失败", "已经有同名的端口(或属性)，新建失败.", QMessageBox::Yes);
            return;
        }
    }
    this->temp_property[propertie_name] = m;
    this->ac.setParam(this->temp_property);
    Utils::alert(qApp->desktop()->rect(), "属性保存成功");
    //更新窗口显示
    this->set_left_list();
    //  跟新后再定位选中
    QListWidgetItem* item0 = w_left->findItems(propertie_name, Qt::MatchExactly).first();
    w_left->setCurrentItem(item0);
    emit w_left->itemClicked(item0);
}

void Component_info_dialog::on_pushButton_save_clicked()
{
    this->pushButton_save(true);
}
//属性删除按钮槽函数
void Component_info_dialog::on_pushButton_delete_clicked()
{
    QMessageBox messageBox(QMessageBox::Warning,
                           "删除", "你确定要删除这个属性吗?",
                           QMessageBox::Yes | QMessageBox::No);
    int result = messageBox.exec();
    switch (result)
    {
    case QMessageBox::Yes:
    {
        QMap<QString, QMap<QString, QString> >pmapthis = this->ac.getParam();
        pmapthis.remove(ui->listWidget->selectedItems().first()->text());
        this->ac.setParam(pmapthis);
        //更新窗口显示
        this->set_left_list();
        if (ui->listWidget->count() != 0)
        {
            //  默认第一个选中
            QListWidgetItem* item0 = ui->listWidget->item(0);
            ui->listWidget->setCurrentRow(0);
            emit ui->listWidget->itemClicked(item0);
        }
        else
        {
            //更新窗口显示
            this->set_right_info(false, "", true, false);
        }
        break;
    }
    case QMessageBox::No:
        //更新窗口显示
        this->set_left_list();
        qDebug() << "NO";
        break;
    default:
        break;
    }

}

void Component_info_dialog::on_pushButton_p_s_clicked()
{
    this->pushButton_save(false);
}
//和属性的按钮重复 懒得变成统一的方法
void Component_info_dialog::on_pushButton_p_d_clicked()
{
    QMessageBox messageBox(QMessageBox::Warning,
                           "删除", "你确定要删除这个端口吗?",
                           QMessageBox::Yes | QMessageBox::No);
    int result = messageBox.exec();
    switch (result)
    {
    case QMessageBox::Yes:
    {
        QMap<QString, QMap<QString, QString> >pmapthis = this->ac.getParam();
        pmapthis.remove(ui->listWidget_2->selectedItems().first()->text());
        this->ac.setParam(pmapthis);
        //更新窗口显示
        this->set_left_list();

        if (ui->listWidget_2->count() != 0)
        {
            //  默认第一个选中
            QListWidgetItem* item0 = ui->listWidget_2->item(0);
            ui->listWidget_2->setCurrentRow(0);
            emit ui->listWidget_2->itemClicked(item0);
        }
        else
        {
            //更新窗口显示
            this->set_right_info(false, "", false, false);
        }
        break;
    }
    case QMessageBox::No:
        //更新窗口显示
        this->set_left_list();
        qDebug() << "NO";
        break;
    default:
        break;
    }
}
//点击 对话框 的 ok/Cancel
void Component_info_dialog::on_buttonBox_clicked(QAbstractButton *button)
{
//    qDebug()<<button->text();
    if(QString::compare("OK", button->text()) == 0)
    {

        //shao：信息合法性验证
        if(QString::compare("", this->ui->lineEdit_sign->text()) == 0)
        {
            Utils::alert(qApp->desktop()->rect(), "信息不完整，请将信息补充完整！");
            return;
        }
        if(QString::compare("", this->ui->lineEdit_name->text()) == 0)
        {
            Utils::alert(qApp->desktop()->rect(), "信息不完整，请将信息补充完整！");
            return;
        }
        else
        {
            QString component_name = this->ui->lineEdit_name->text();
            QByteArray ba = component_name.toLatin1();//QString 转换为 char*
            for(int i = 0 ; i < ba.length(); i++)
            {
                if(!((ba[i] >= '0' && ba[i] <= '9') ||
                        (ba[i] >= 'A' && ba[i] <= 'Z') ||
                        (ba[i] >= 'a' && ba[i] <= 'z') ||
                        ba[i] == '_'))
                {
                    Utils::alert(qApp->desktop()->rect(), "组件名称不规范");
                    return;
                }
            }
            // shao:首字母不能为数字
            if((ba[0] >= '0' && ba[0] <= '9') || ba[0] == '_')
            {
                Utils::alert(qApp->desktop()->rect(), "组件名称不能数字或下划线开头");
                return;
            }
            //shao:名称不能重复
            QList<QString> name_list = Component_utile::read_file_get_name_list();
            if(if_new_component&&name_list.count(component_name))
            {
                Utils::alert(qApp->desktop()->rect(), "组件名称重复！");
                return;
            }
        }

        //保存信息
        this->save_info_page();

        if(this->flag == 0)
        {
            // 重新写入文件
            Utils::writeAlgorithmComp2Xml(this->ac);
            //        Utils::alert(qApp->desktop()->rect() , "组件修改成功");
            QMessageBox messageBox(QMessageBox::Information,
                                   "成功", "算法组件修改成，关闭对话框?",
                                   QMessageBox::Yes | QMessageBox::No);
            int result = messageBox.exec();
            switch (result)
            {
            case QMessageBox::Yes:
            {
                this->close();
                emit this->succeed(this->ac.getFileName());//去更新窗口
                break;
            }
            case QMessageBox::No:
                break;
            }
        }
        else if(this->flag == 1)
        {
            emit edit_succeed(&this->ac);
            QMessageBox messageBox(QMessageBox::Information,
                                   "成功", "算法组件属性设置，关闭对话框?",
                                   QMessageBox::Yes | QMessageBox::No);
            int result = messageBox.exec();
            switch (result)
            {
            case QMessageBox::Yes:
            {
                this->close();
                break;
            }
            case QMessageBox::No:
                break;
            }
        }
    }
    else
    {
        QMessageBox messageBox(QMessageBox::Warning,
                               "取消", "你确定要取消对这个算法组件的修改吗?",
                               QMessageBox::Yes | QMessageBox::No);
        int result = messageBox.exec();
        switch (result)
        {
        case QMessageBox::Yes:
        {
            this->close();
            break;
        }
        case QMessageBox::No:
            break;
        }
    }
}

void Component_info_dialog::on_pushButton_Path_clicked()
{
    QString dirpath = QDir::currentPath();
    QString path = QFileDialog::getExistingDirectory(this, "选择文件夹", QString(dirpath), QFileDialog::ShowDirsOnly);
    ui->pushButton_Path->setText(path);
}

int Component_info_dialog::getFlag() const
{
    return flag;
}

void Component_info_dialog::setFlag(int value)
{
    flag = value;
}
