#include "simdatagenwidget.h"
#include "ui_simdatagenwidget.h"
#include "SimDataGen.cpp"
#include "utils.h"
#include <QDate>
#include <QDir>
#include <QException>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QDesktopWidget>

/**
 * @brief 模拟数据生成窗口
 * @param parent 父类
 */
SimDataGenWidget::SimDataGenWidget(QString curr_eq, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SimDataGenWidget),
    curr_equ(curr_eq)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
}

SimDataGenWidget::~SimDataGenWidget()
{
    delete ui;
}

#if 0
/**
 * @brief 选择文件
 */
void SimDataGenWidget::on_pushButton_select_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择文件夹"),
                   QDir::currentPath(),
                   tr("text (*.txt)"));
    ui->lineEdit_path->setText(path);
    ui->pushButton_select->setText("OK");
}

#endif
/**
 * @brief OK
 */
void SimDataGenWidget::on_buttonBox_accepted()
{
    qDebug() << "点击ok";
    close();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-19
 * @brief 加载目标模型按钮，用来生成数据到INPUT文件夹中的数据
 */
void SimDataGenWidget::on_pushButton_load_clicked()
{
    //信号参数-数据结构
    SIMRE_LIB_PARAM params;
    // 赋值
    params.nPRT =  ui->lineEdit_period->text().toInt();			//雷达重复周期为脉冲重复频率(4K)的倒数 单位us
    params.nPW = ui->lineEdit_width->text().toInt();			//脉宽10us 单位us
    params.nPulse = ui->lineEdit_child->text().toInt();			//一组脉冲中的子脉冲个数：16, 单位个
    params.nBand = ui->lineEdit_bandwidth->text().toInt();			//调频带宽：50M 单位MHz
    params.nDeltaf = ui->lineEdit_step->text().toInt();		//调频带宽步进频率：暂取10 单位MHz
    params.nSampling = ui->lineEdit_datasamp->text().toInt();		//采样率：60MHz 单位MHz

    params.nGroup = ui->lineEdit_group->text().toInt();			//脉冲组数 单位个
    params.nRF = ui->lineEdit_freq->text().toInt();			//雷达载频即发射频率：10GHz 单位GHz
    params.amplitude = ui->lineEdit_amplitude->text().toInt();		//目标回波幅度： 单位m
    params.range = ui->lineEdit_posi->text().toInt();		//目标径向位置 单位km
    params.velcity = ui->lineEdit_speed->text().toInt();		//目标径向速度	 单位m
    params.omega = ui->lineEdit_angular->text().toInt();			//目标旋转角 单位度
    params.acc = ui->lineEdit_acceler->text().toInt();				//目标径向加速度 单位m

    qDebug() << "mmmmmmmmmmm数据生成";
#if 0
    {
        // TODO 存在哪个文件夹应该能自动判别出，比如"雷达6"--->"/radar"
        QString path = QDir::currentPath() + "/radar/" + curr_equ + "/room/waveData/";
        QString inputdir = Utils::getOutputOrInputDirName(path, "INPUT");

        if(inputdir.compare("error") == 0)
        {
            // TODO 数据文件生成保存路径和判断逻辑要改
            qDebug() << "waveData目录下INPUT文件夹出现问题，注意!";
            Utils::alert(qApp->desktop()->rect(), "数据无法生成，等待后续更改实现逻辑");
        }
        else
        {
            // 这里不让用户自己填写了，默认就是本工程的空间中
            QString s = path + inputdir + "/data.txt";
            SimDataGen sdg(params);

            connect(this, &SimDataGenWidget::geneData, [&](QString s)
            {
                qDebug() << "generate data begin" << s;
                // 这里卡了我一个晚上，因为SimDataGen文件中的fopen函数写文件数据的时候不支持中文路径，而雷达名字是带有中文的。。
                //        qDebug() << "dayin!!----" << const_cast<char *>(s.toLocal8Bit().toStdString().c_str());
                //        sdg.m_echo = const_cast<char *>(s.toLocal8Bit().toStdString().c_str());
                // 目前只有这样传参数进去才可以在Ubuntu上面正常生成数据，否则使用成员初始化列表和对象传参都是乱码
                sdg.GenData(const_cast<char *>(s.toLocal8Bit().toStdString().c_str()));
            });
            // 没有则创建目录
            Utils::openDirOrCreate(path + inputdir);
            ui->lineEdit_path->setText(s);
            // 打开文件
            QFile file(s);
            if(file.exists())
            {
                qDebug() << "输入数据文件已存在";
            }
            else
            {
                qDebug() << "文件不存在,正在新建文件.";
                if(!file.open(QIODevice::ReadWrite))
                {
                    QMessageBox::warning(this, "文件名设置出错！", "请检查文件名的格式是否正确！");
                    return;
                }
                qDebug() << "新建成功";
            }
            file.close();
            emit geneData(s);

//        #if 0
            QDir cdir(QDir::currentPath());
            cdir.cdUp();
            cdir.cd(qAppName() + "/Projects/test/code");
            // 如果是打包成可执行文件，则需要这样写
//        #endif
//      QDir cdir(QDir::currentPath() + "/Projects/test/code");
            QFile ffile(cdir.absolutePath() + "/data/INPUT.txt"); //将要复制到的新文件完整路径
            //判断文件按是否存在，存在则删除
            if(ffile.exists())
            {
                qDebug() << ffile.fileName() << "data里面的INPUT文件已存在，先删除";
                ffile.remove();
            }
            if(QFile::copy(s, cdir.absolutePath() + "/data/INPUT.txt")) //将文件复制到新的文件路径下
            {
                qDebug() << QStringLiteral("INPUT文件复制成功");
            }
            else
                qDebug() << QStringLiteral("INPUT文件复制失败");
            // FIXME 上面这里有个问题,就是文件复制过去最后还得复制回来
            ui->pushButton_load->setText("数据生成完成");
            // 生成之后就不能再重复点击了
            ui->pushButton_load->setEnabled(false);
            clearFocus();
            Utils::alert(qApp->desktop()->rect(), "模拟数据生成完成");
        }
    }
#endif
}

void SimDataGenWidget::on_buttonBox_rejected()
{
    qDebug() << "cancle";
    close();
}

