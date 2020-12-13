#include "compproperty.h"
#include "ui_compproperty.h"
#include "utils.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>

CompProperty::CompProperty(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompProperty),
    iofName(name)
{
    ui->setupUi(this);
    this->setWindowTitle("设置"+name+"文件路径");
    setPath(path_);
    ui->label->setText(name);
}

CompProperty::~CompProperty()
{
    delete ui;
}

/**
 * @brief 用户选择路径
 */
void CompProperty::on_pushButton_clicked()
{
//    QString dirpath = "/home/a913/Qt_project/20191130/prototype_v0906/Projects/test/code/";
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd(qAppName()+"/Projects/test/code");
    QString dirpath = dir.absolutePath();
//    QString path = QFileDialog::getExistingDirectory(this, "选择文件夹", QString(dirpath), QFileDialog::ShowDirsOnly);
    QString path = QFileDialog::getOpenFileName(this, tr("选择文件"),
                                                    QString(dirpath),
                                                    tr("text (*.txt *.xml)"));
    path_ = path;
    ui->label_path->setText(path);
    ui->pushButton->setText("OK");
    ui->pushButton->setEnabled(false);
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-28
 * @brief CompProperty::on_pushButton_2_clicked 点击ok
 */
void CompProperty::on_pushButton_2_clicked()
{
    Utils::modifyCompXmlPath(QDir::currentPath()+"/radar/"+getEquipId()+"/room/algoXml/"+iofName+getUid().mid(1,getUid().length()-2)+".xml", path_, getEquipId(),scene);
    this->close();
}

QString CompProperty::getPath() const
{
    return path_;
}

void CompProperty::setPath(const QString &path)
{
    if(!path.isNull()){
        path_ = path;
        ui->label_path->setText(path_);
    }
}

QGraphicsScene *CompProperty::getScene() const
{
    return scene;
}

void CompProperty::setScene(QGraphicsScene *value)
{
    scene = value;
}

QString CompProperty::getEquipId() const
{
    return equipId;
}

void CompProperty::setEquipId(const QString &value)
{
    equipId = value;
}

QString CompProperty::getUid() const
{
    return uid;
}

void CompProperty::setUid(const QString &value)
{
    uid = value;
}
