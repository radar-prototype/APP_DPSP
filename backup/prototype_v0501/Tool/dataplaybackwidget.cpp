#include "dataplaybackwidget.h"
#include "ui_dataplaybackwidget.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>

DataPlayBackWidget::DataPlayBackWidget(QString window_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataPlayBackWidget),
    winName(window_name)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->groupBox_file_set->hide();

    ui->progressBar_load->setRange(0, 100);
    ui->progressBar_load->setValue(0);
}

DataPlayBackWidget::~DataPlayBackWidget()
{
    delete ui;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_radar_add_clicked 添加雷达数据文件
 */
void DataPlayBackWidget::on_pushButton_radar_add_clicked()
{
    // 用户选择dpsp的路径
    QStringList nameList = QFileDialog::getOpenFileNames(this, tr("选择添加radar数据"), QDir::currentPath() + "/project/" + winName + "/data/", "Data Files(*.txt)");
//    QString ppath = QFileDialog::getOpenFileName(nullptr, tr("选择添加radar数据"), QDir::currentPath()+"/project/"+winName+"/data/", "Data Files(*.txt)");
    if(!nameList.isEmpty())
    {
        for(auto item : nameList)
            ui->listWidget_rad->addItem(QFileInfo(item).baseName() + ".txt");
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_radar_del_clicked 点击删除选中的项（单选）
 */
void DataPlayBackWidget::on_pushButton_radar_del_clicked()
{
    // 获取当前选中的item，删除
    QList<QListWidgetItem*> sl = ui->listWidget_rad->selectedItems();
    if(!sl.isEmpty())
        ui->listWidget_rad->takeItem(ui->listWidget_rad->row(sl.first()));
    else
        qDebug() << "未选中,不删除";
}

void DataPlayBackWidget::on_pushButton_set_toggled(bool checked)
{
    if(checked)
    {
        ui->groupBox_file_set->showNormal();
        ui->pushButton_set->setText("隐藏导入设置");
        QDesktopWidget *deskdop = QApplication::desktop();
        this->move((deskdop->width() - this->width()) / 2, (deskdop->height() - height()) / 2);
    }
    else
    {
        ui->groupBox_file_set->hide();
        ui->pushButton_set->setText("导入文件设置");
        QDesktopWidget *deskdop = QApplication::desktop();
        this->move((deskdop->width() - 250) / 2, (deskdop->height() - 200) / 2); //高正中间是184
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_apply_clicked 应用上述所有设置， 不关闭窗口，apply按钮不可用，知道设置有更改
 */
void DataPlayBackWidget::on_pushButton_apply_clicked()
{
    // TODO 保存设置
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_cancel_clicked 取消一切设置，直接关闭窗口
 */
void DataPlayBackWidget::on_pushButton_cancel_clicked()
{
    // TODO 取消上述设置
    this->close();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_yes_clicked 应用上述所有设置，关闭窗口
 */
void DataPlayBackWidget::on_pushButton_yes_clicked()
{
    // TODO 保存设置

    // 关闭窗口、
    close();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-11
 * @brief DataPlayBackWidget::on_pushButton_load_clicked 加载地图或其他跟仿真设备无关的数据，比如场景等渲染过程，进度条变化
 */
void DataPlayBackWidget::on_pushButton_load_clicked()
{
    QProgressBar *load = ui->progressBar_load;
    load->setValue(98);
    // TODO 读取全局数据，渲染场景初始参数
    load->setValue(100);
}
