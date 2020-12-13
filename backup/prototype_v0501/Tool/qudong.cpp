#include "qudong.h"
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QDir>

// This is an auto-generated comment by Antrn.
/************************************
 * @author Antrn                    *
 * @date 2020-09-30 T 16:13:51      *
 * @description      待删，老版本用的代码，之后不知道会不会用的到
 * @copyright (c)                   *
 ************************************/
QuDong::QuDong()
{

}

QuDong::QuDong(QList<QString> nameandidlist, QString equipName): equipName(equipName), namelist(nameandidlist)
{

}

void QuDong::startRun(QString fileName, QString outName, QString runIt)
{
//    QProcess *p1 = new QProcess;//./home/a913/Qt_project/20190910/prototype_v0906/Projects/test/code/radar_test.out
//    p1->start("g++", QStringList() << "/home/a913/Qt_project/20190910/prototype_v0906/Projects/test/code/radar_test.cpp"<< "-fopenmp"<< "-lfftw3"<< "-lm"<< "-o" <<"/home/a913/Qt_project/20190910/prototype_v0906/Projects/test/code/radar_test.out");
//    p1->waitForStarted();
//    p1->close();
//    p1->waitForFinished();
    QStringList args;
    args.append(fileName);
    args.append("-o");
    args.append(outName);
//    args.append("-lvsip");
//    args.append("-lfftw3");
    args.append("-lfftw3f");
    args.append("-lvsip");
    args.append("-w");
    args.append("-lm");
//    args.append("-lpthread");

    QString command = args.join(" ");
    qDebug() << "command: " << command;

    QProcess *p1 = new QProcess;
    p1->start("gcc", args, QIODevice::ReadWrite);
    p1->waitForStarted();

    p1->waitForFinished();

    QString strTemp = QString::fromLocal8Bit(p1->readAllStandardOutput());
    QString strError = QString::fromLocal8Bit(p1->readAllStandardError());

    p1->close();

    p1->kill();

    QProcess *p = new QProcess;
    // 修改此处的参数
//    QString workpath = "/home/a913/Qt_project/20191130/prototype_v1202/Projects/test/code/";
//#if 0
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cd(qAppName() + "/Projects/test/code/");
    // 如果是打包成可执行文件，则需要这样写
//#endif
//    QDir dir(QDir::currentPath() + "/Projects/test/code");

    qDebug() << "startRun here" << dir;
    QString workpath = dir.absolutePath();
    qDebug() << "-----" << workpath;
    p->setWorkingDirectory(workpath);
    p->start(runIt);
    p->waitForStarted();
    p->waitForFinished();

    strTemp += QString::fromLocal8Bit(p->readAllStandardOutput());
    strError += QString::fromLocal8Bit(p->readAllStandardError());


    p->close();
    p->kill();

    // 输出控制台信息
    qDebug() << "输出信息：" << strTemp << ";" << endl << "错误信息:" << strError;
    QMessageBox testMassage;
    testMassage.setWindowTitle("信息提示");
    testMassage.setText(strTemp + strError);
    testMassage.exec();

    // 将数据文件从Projects/test/code分发回来
    QDir data_dir(workpath + "/data");
    qDebug() << "数据文件" << data_dir.absolutePath();
    //要判断路径是否存在
    if(!data_dir.exists())
    {
        qDebug() << "data dir it is not true path";
        return ;
    }
    data_dir.setFilter(QDir::Files);//实现对文件夹的过滤

    QFileInfoList list = data_dir.entryInfoList();
    QString desdir = QDir::currentPath() + "/radar/" + equipName + "/room/waveData/";
    qDebug() << "目标文件夹:" << desdir;
    qDebug() << "设备" << equipName << "list" << namelist;
    for (int i = 0; i < list.size(); ++i)
    {
        qDebug() << i;
        QFileInfo fileInfo = list.at(i);
        QString name = fileInfo.completeBaseName();
        qDebug() << "fileName" << name;
        for (QString s : namelist)
        {
            if(s.contains(name))
            {
                qDebug() << "要复制文件的路径" << fileInfo.filePath() << "目标文件名" << s;
                QFile ffile(desdir + s + "/data.txt"); //将要复制到的新文件完整路径
                //判断文件按是否存在，存在则删除
                if(ffile.exists())
                {
                    qDebug() << ffile.fileName() << "文件已存在，先删除";
                    ffile.remove();
                }
                if(QFile::copy(fileInfo.filePath(), desdir + s + "/data.txt")) //将文件复制到新的文件路径下
                {
                    qDebug() << QStringLiteral("data文件复制成功");
                }
                else
                    qDebug() << QStringLiteral("data文件复制失败");
            }
        }
    }
}

/**
 * @brief QuDong::startRunCpp
 * @param fileName 执行代码的完整文件名
 * @param outName 输出文件名，一般是.o
 * @param radar_name 雷达ID，即雷达名字
 */
void QuDong::startRunCpp(QString fileName, QString outName, QString radar_name)
{
    QStringList args;
    args.append(fileName);
    args.append("-o");
    args.append(outName);

    QProcess *p1 = new QProcess;
    p1->start("gcc", args);
    p1->waitForStarted();
    p1->waitForFinished();

    QString strTemp = QString::fromLocal8Bit(p1->readAllStandardOutput());
    QString strError = QString::fromLocal8Bit(p1->readAllStandardError());

    p1->close();
    p1->kill();

    QProcess *p = new QProcess;
    QString workpath = QDir::currentPath() + "/radar/" + radar_name + "/room/";
//    p->setWorkingDirectory(workpath);
//    qDebug() << "环境是" << p->environment();
    p->start(workpath + "/" + radar_name + ".exe");
    p->waitForStarted();
    p->waitForFinished();

    strTemp += QString::fromLocal8Bit(p->readAllStandardOutput());
    strError += QString::fromLocal8Bit(p->readAllStandardError());

    p->close();
    p->kill();

    // 输出控制台信息
//    qDebug() << "输出信息： " << strTemp << ";" << strError;
    QMessageBox testMassage;
    testMassage.setText(strTemp + strError);
    testMassage.setWindowTitle("输出信息");
    testMassage.exec();
}
