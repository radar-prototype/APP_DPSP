#ifndef QUDONG_H
#define QUDONG_H
#include <QList>
#include <QString>

using namespace std;
/**
 * @brief The QuDong class 代码编译时执行指令的类
 */
class QuDong
{
public:
    QuDong();
    QuDong(QList<QString>nameandidlist, QString equipName);
    void startRun(QString fileName, QString outName, QString runIt);
    static void startRunCpp(QString fileName, QString outName, QString radar_name);
private:
    QString equipName;
    QList<QString> namelist;
};

#endif // QUDONG_H
