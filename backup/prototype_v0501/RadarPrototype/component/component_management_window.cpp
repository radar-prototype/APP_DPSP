#include "component_management_window.h"
#include "ui_component_management_window.h"
#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include "utils.h"
#include "component_info_dialog.h"
Component_management_window::Component_management_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Component_management_window)
{
    ui->setupUi(this);
    //ui优化
    ui->label_1 ->setWordWrap(true);
    ui->label_1->setFrameShadow(QFrame::Sunken);
    ui->label_1->setFrameShape(QFrame::Panel);
    ui->label_1->setAlignment(Qt::AlignCenter);
    ui->label_2->setWordWrap(true);
    ui->label_2->setFrameShadow(QFrame::Sunken);
    ui->label_2->setFrameShape(QFrame::Panel);
    ui->label_2->setAlignment(Qt::AlignCenter);

    //左边的算法组件列表
    this->component_list_left = new class Component_list_left(this);
    //读取默认算法组件列表
    this->component_list_left->loadAllComps(ui->widget_1_list);
    //中间的算法信息
    this->component_info = new class Component_info(this);
    //右边的图案显示
    this->component_display_right = new class Component_display_right(this);

//切换item
    connect(component_list_left, &Component_list_left::clickItem, this, [&](QListWidgetItem * item)
    {

        //接收指针
        QMap<QString, AlgorithmComp>* algorithms = (QMap<QString, AlgorithmComp>*)item->data(Qt::UserRole).value<void*>();

        //每次切换都要更新 根据 id取出 组件
        this->algorithmComp = algorithms->value(item->data(Qt::UserRole + 2).toString());

        //显示算法组件信息
        component_info->setInfoWindow(ui->widget_2_base, &this->algorithmComp);
        //显示右边图形
        component_display_right->displayCompenent(ui->widget_4_1, &this->algorithmComp);
    });
    //默认第一个选中
    QListWidgetItem* item0 = this->component_list_left->get_listWidget()->item(0);
    if (item0 != nullptr) emit component_list_left->clickItem(item0);

//    connect(this->component_info_dialog,&component_info_dialog::succeed,this,[&](QString name_component){
//        qDebug()<<"ssssss";
//        component_list_left->update_component(name_component);

//    });

}
Component_management_window::~Component_management_window()
{
    delete ui;
}


void Component_management_window::on_pushButton_add_clicked()
{
    Component_info_dialog *component_info_dialog = new class Component_info_dialog(this);
    component_info_dialog->add_dialog(&this->algorithmComp);
    connect(component_info_dialog, &Component_info_dialog::succeed, this, [&](QString name_component)
    {
        component_list_left->update_component(name_component);
    });
}

void Component_management_window::on_pushButton_import_clicked()
{
    // 导入文件
    QString dirpath = QDir::currentPath();
    Utils::openDirOrCreate(dirpath);
    // 打开xml文件读取
    const QString fileName = QFileDialog::getOpenFileName(this, tr("打开组件xml"), QString(dirpath), tr("xml files (*.xml)"));

//    qDebug() << "长名字：" << fileName;
    if(QString::compare("", fileName) == 0)
    {
        return;
    }
    //                QFileInfo fi = QFileInfo(fileName);
    //                QString file_name = fi.fileName().split(".")[0];  //获取文件名
    //                qDebug() << "短名字： " << file_name;
    AlgorithmComp ac = Utils::readPluginXmlFile(fileName);
    QString file_name = ac.getFileName();
    // 已经有了
    if(this->component_list_left->getNameList().contains(file_name))
    {
        qDebug() << "已经导入了! id:" << ac.getInfo()["ID"] << "组件名字: " << ac.getInfo()["Name"];
        QMessageBox::warning(nullptr, "提醒",
                             "不能导入具有相同名字的组件入库！",
                             QMessageBox::Yes);
    }
    else
    {
        // 将这里的文件复制过来
        QFile::copy(fileName, dirpath + "/algoXml/" + file_name + ".xml");
        this->component_list_left->update_component(file_name);

    }
}

void Component_management_window::on_pushButton_change_clicked()
{
    Component_info_dialog *component_info_dialog = new class  Component_info_dialog(this);
    component_info_dialog->edit_dialog(&this->algorithmComp);
    connect(component_info_dialog, &Component_info_dialog::succeed, this, [&](QString name_component)
    {
        component_list_left->update_component(name_component);

    });
}

void Component_management_window::on_pushButton_delete_clicked()
{
    QListWidgetItem * item = this->component_list_left->get_listWidget()->currentItem();
    if( item == nullptr )
        return;
    // FIXME 这里也是用字符串匹配的  (shao：应该怎么匹配？)
    if(item->text().compare("INPUT") == 0 || item->text().compare("OUTPUT") == 0)
    {
        qDebug() << "不允许删除输入输出组件!";
        QMessageBox *mb = new QMessageBox(nullptr);
        mb->setWindowTitle("警告");
        mb->setText("抱歉！您不能删除内置：‘" + item->text() + "’组件！");
        mb->setIcon(QMessageBox::Warning);
        mb->show();
        return;
    }
    QMessageBox *mb = new QMessageBox(nullptr);
    mb->setWindowTitle("警告");
    mb->setText(" 您确定要删除" + item->text() + "’组件吗！");
    mb->setIcon(QMessageBox::Warning);
    QCheckBox* check_box = new QCheckBox("勾选同时删除本地文件", this);
    mb->setCheckBox(check_box);
    mb->setStandardButtons(QMessageBox::Yes |  QMessageBox::Cancel);
    mb->setDefaultButton(QMessageBox::Yes);
    int ch = mb->exec();
    if ( ch != QMessageBox::Yes )
        return;
    else
    {
        QString na = item->text();
        QString id = item->data(Qt::UserRole + 2).toString();
        qDebug() << "组件库-删除组件: " << na;
        // nameList名字删除
        this->component_list_left->get_nameList()->removeOne(na);
        // 这里面也要删
        this->component_list_left->get_algorithms()->take(id);
        // 对应文件也要删除
        if(check_box->isChecked())
        {
            qDebug() << "文件也要删除";
            QList<QString> codes =
            {
                QDir::currentPath() + "/algoXml/" + na.toLower() + ".xml",
                QDir::currentPath() + "/codes/" + na.toLower() + ".c",
                QDir::currentPath() + "/codes/" + na.toLower() + ".h",
                QDir::currentPath() + "/codes/" + na.toLower() + ".txt"
            };
            for(int i = 0; i < 4; i++)
            {
                QFile file(codes.at(i));
                file.remove();
                file.close();
            }
        }
        delete item;
        //默认第一个选中
        QListWidgetItem* item0 = this->component_list_left->get_listWidget()->item(0);
        if (item0 != nullptr) emit component_list_left->clickItem(item0);
    }
}
