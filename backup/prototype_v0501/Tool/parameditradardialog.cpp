#include "algorithmcomp.h"
#include "parameditradardialog.h"
#include "ui_parameditradardialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QTableWidget>
#include "utils.h"
#include <QApplication>
#include <QDesktopWidget>
#include <radarcompdraglistwidget.h>



//ParamEditRadarDialog::ParamEditRadarDialog(QString fname, QWidget *parent) :
/**
 * 参数编辑构造函数
 * @brief ParamEditRadarDialog::ParamEditRadarDialog
 * @param ac
 * @param parent
 */
ParamEditRadarDialog::ParamEditRadarDialog(AlgorithmComp ac, QWidget *parent, int flag) :
    QDialog(parent),
    ac(ac),
    flag(flag),
    ui(new Ui::ParamEditRadarDialog)
//    fname(fname),
{
    ui->setupUi(this);
    // 重新设置时间，不然不对
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    // 必须要设置有多少行，在ui中设置也行
//    ui->tableWidget_Param->setRowCount(20);
//    ui->tableWidget_Param->setColumnCount(3);
    ui->tableWidget_Param->setShowGrid(true);
    QTableWidgetItem *columnHeaderItem0 = ui->tableWidget_Param->horizontalHeaderItem(0); //获得水平方向表头的第一个Item对象
    columnHeaderItem0->setBackgroundColor(QColor(0, 60, 10)); //设置单元格背景颜色
    columnHeaderItem0->setTextColor(QColor(200, 111, 30)); //设置文字颜色
    ui->tableWidget_Param->setFont(QFont("Helvetica")); //设置字体
    //去掉水平滚动条
//    ui->tableWidget_Param->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //垂直滚动条按项移动
    ui->tableWidget_Param->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    //去掉自动滚动
//    ui->tableWidget_Param->setAutoScroll(false);
    ui->tableWidget_Param->horizontalHeader()->setStretchLastSection(true);
    // 选中单个单元格
    ui->tableWidget_Param->setSelectionBehavior(QAbstractItemView::SelectItems);
//    ui->tableWidget_Param->setSelectionMode(QAbstractItemView::SingleSelection);
//    ui->tableWidget_Param->setSelectionMode(QAbstractItemView::MultiSelection);
    // 正常情况下是单选，按下Ctrl键后，可以多选
    ui->tableWidget_Param->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // 固定宽度先
    ui->pushButton_Path->setFixedWidth(335);
    // 双击可修改
    ui->tableWidget_Param->setEditTriggers(QAbstractItemView::DoubleClicked);

    // 每次重新读取文件不行，应该获取id，从algorithms里面找到ac
//    if(fname.compare("null")){
//        AlgorithmComp ac = Utils::readPluginXmlFile(QDir::currentPath()+"/algoXml/"+fname+".xml");
    if(!ac.getInfo()["ID"].isEmpty())
    {
        QString id = ac.getInfo()["ID"];
//        if(id.isEmpty() || id.isNull()){
//            // 生成唯一uuid
//            ui->lineEdit_ID->setText(QUuid::createUuid().toString());
//        }
        this->ui->lineEdit_Name->setText(ac.getInfo()["Name"]);
        this->ui->lineEdit_CN_Name->setText(ac.getInfo()["CN_Name"]);//获取中文名
        this->ui->lineEdit_Fun->setText(ac.getInfo()["callFunction"]);//获取函数调用
        // WARNING 不能编辑xml文件名字，因为它是组件的名字，修改了的话保存的时候以新名字为准，就会重新生成一个新的组件文件
//        ui->lineEdit_Name->setReadOnly(true);
        ui->lineEdit_ID->setText(id);
        ui->pushButton_Path->setText(ac.getInfo()["Path"]);
        ui->textEdit->setText(ac.getDesc());
//        qDebug() << QDateTime::fromString(QString(ac.getInfo()["Time"]), "M/d/yyyy h:mm AP");
        ui->dateTimeEdit->setDateTime(QDateTime::fromString(QString(ac.getInfo()["Time"]), "M/d/yyyy h:mm AP"));

        // 取出所有的参数map
        QMap<QString, QMap<QString, QString>> paraMap = ac.getParam();
        for (QMap<QString, QMap<QString, QString>>::iterator j = paraMap.begin(); j != paraMap.end(); j++)
        {
            int r = ui->tableWidget_Param->rowCount();//获取表格中当前总行数
            ui->tableWidget_Param->setRowCount(r + 1); //添加一行
            row = r;
            QTableWidgetItem *itemName, *itemDesc, *itemValue, *itemIsoutput, *itemType;
            itemName = new QTableWidgetItem;
            itemDesc = new QTableWidgetItem;
            itemValue = new QTableWidgetItem;
            itemIsoutput = new QTableWidgetItem;
            itemType = new QTableWidgetItem;
            itemName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //居中
            itemDesc->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemValue->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemIsoutput->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            itemType->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            QString txt = QString("%1").arg(j.key());
            itemName->setText(txt);
            ui->tableWidget_Param->setItem(row, 0, itemName);
            txt = QString("%1").arg(j.value().value("describe"));
            itemDesc->setText(txt);
            ui->tableWidget_Param->setItem(row, 1, itemDesc);
            txt = QString("%1").arg(j.value().value("type"));
            itemType->setText(txt);
            ui->tableWidget_Param->setItem(row, 2, itemType);
            txt = QString("%1").arg(j.value().value("isoutput"));
            itemIsoutput->setText(txt);
            ui->tableWidget_Param->setItem(row, 3, itemIsoutput);
            txt = QString("%1").arg(j.value().value("value"));
            itemValue->setText(txt);
            ui->tableWidget_Param->setItem(row, 4, itemValue);

            row++;
        }
    }
    else
    {
        // 生成唯一uuid
        ui->lineEdit_ID->setText(QUuid::createUuid().toString());
        qDebug() << "组件的id为空，生成新的id： " << ui->lineEdit_ID->text();
    }
    // 不能修改
    ui->lineEdit_ID->setEnabled(false);
}

ParamEditRadarDialog::~ParamEditRadarDialog()
{
    delete ui;
}

/**
 * @brief 自动根据文字生成图片的功能在这里
 * 点击确定，完成新建组件的过程
 */
void ParamEditRadarDialog::on_pushButton_OK_clicked()
{
    QString name_algo = ui->lineEdit_Name->text();

    // 这里要判断一下name是否合法
    if(name_algo == nullptr || name_algo.isEmpty())
    {
        emit showMessage("[警告]： 拒绝添加！组件名不能为空", 0);
        reject();
    }
    else if(dynamic_cast<RadarCompDraglistWidget *>(this->parent())->nameList.contains(name_algo) && flag == 0)
    {
        qDebug() << "与现有的文件名: " << name_algo << "重复，添加失败！";
        emit showMessage("[警告]： 已有重复名称存在，请重新新建组件！", 0);
        reject();
    }
    else
    {
        ac.setDesc(ui->textEdit->toPlainText());
        mp.insert("ID", ui->lineEdit_ID->text());

        QString nm = ui->lineEdit_Name->text();
        mp.insert("Name", nm);
        // 生成在本雷达下的icon图像
        emit sendIconName(nm);

        // 只有第一次初始化用这个，后面的编辑就不改变了
        if(ac.getFileName().isEmpty() || ac.getFileName().isNull())
        {
            ac.setFileName(nm);
        }
        mp.insert("CN_Name", ui->lineEdit_CN_Name->text()); //中文名称
        mp.insert("callFunction", ui->lineEdit_Fun->text()); //函数
        mp.insert("Time", ui->dateTimeEdit->dateTime().toString());
        mp.insert("Path", ui->pushButton_Path->text());
        ac.setInfo(mp);
        QMap<QString, QMap<QString, QString> >pmap;
        QMap<QString, QString> m;
        qDebug() << "参数总行数row: " << row;
        for (int i = 0; i < row; i++)
        {
            // WARNING 这里填写的内容一定不能为纯数字！！！，xue的教训，会直接造成写入成功，读取失败!
            QString name = ui->tableWidget_Param->item(i, 0)->text();
            QString desc = ui->tableWidget_Param->item(i, 1)->text();
            QString type = ui->tableWidget_Param->item(i, 2)->text();
            QString isoutput = ui->tableWidget_Param->item(i, 3)->text();
            QString value = ui->tableWidget_Param->item(i, 4)->text();
            //        qDebug() << name <<": " << desc <<"; " << type;
            m.clear();
            m.insert("describe", desc);
            m.insert("type", type);
            m.insert("isoutput", isoutput);
            m.insert("value", value);
            pmap.insert(name, m);
        }
//        qDebug() << "参数列表的大小: " << pmap.size();
        ac.clearParam();
        ac.setParam(pmap);
        accept();
    }
}

void ParamEditRadarDialog::on_pushButton_Cancel_clicked()
{
    close();
}

/**
 * 路径选择
 */
void ParamEditRadarDialog::on_pushButton_Path_clicked()
{
    QString dirpath = QDir::currentPath();
    QString path = QFileDialog::getExistingDirectory(this, "选择文件夹", QString(dirpath), QFileDialog::ShowDirsOnly);
    ui->pushButton_Path->setText(path);
}

/**
 * 添加参数
 */
void ParamEditRadarDialog::on_pushButton_Add_clicked()
{
    int r = ui->tableWidget_Param->rowCount();//获取表格中当前总行数
    ui->tableWidget_Param->setRowCount(r + 1); //添加一行
    row = r;
    QTableWidgetItem *itemName, *itemDesc, *itemValue, *itemIsoutput, *itemType;
    itemName = new QTableWidgetItem;
    itemDesc = new QTableWidgetItem;
    itemType = new QTableWidgetItem;
    itemIsoutput = new QTableWidgetItem;
    itemValue = new QTableWidgetItem;

    itemName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter); //居中
    itemDesc->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    itemType->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    itemIsoutput->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    itemValue->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QString txt = QString("%1").arg("参数名");
    itemName->setText(txt);
    ui->tableWidget_Param->setItem(row, 0, itemName);
    txt = QString("%1").arg("参数描述");
    itemDesc->setText(txt);
    ui->tableWidget_Param->setItem(row, 1, itemDesc);
    txt = QString("%1").arg("参数类型");
    itemType->setText(txt);
    ui->tableWidget_Param->setItem(row, 2, itemType);
    txt = QString("%1").arg("0:否1:是2:参数");
    itemIsoutput->setText(txt);
    ui->tableWidget_Param->setItem(row, 3, itemIsoutput);
    txt = QString("%1").arg("");
    itemValue->setText(txt);
    ui->tableWidget_Param->setItem(row, 4, itemValue);

    row++;
}

/**
 * 删除参数
 */
void ParamEditRadarDialog::on_pushButton_Del_clicked()
{
    QList<QTableWidgetItem*> items = ui->tableWidget_Param->selectedItems();
    int curRow;
    for (int i = 0; i < items.count(); i++)
    {
        curRow = ui->tableWidget_Param->row(items.at(i));//获取选中的行
        ui->tableWidget_Param->removeRow(curRow);
    }
    row -= items.count();
}
