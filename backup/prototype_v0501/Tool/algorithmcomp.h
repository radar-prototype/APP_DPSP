#ifndef ALGORITHMCOMP_H
#define ALGORITHMCOMP_H
#include <QMap>

// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介 雷达组件xml对应的类   （统一的）算法组件类
* @author        Antrn
* @date          2019-10-03
*/
class AlgorithmComp
{
public:
    AlgorithmComp();
    QMap<QString, QString> getInfo() const;
    void setInfo(const QMap<QString, QString> &value);

    QString getDesc() const;
    void setDesc(const QString &value);

    QMap<QString, QMap<QString, QString> > getParam() const;
    void setParam(const QMap<QString, QMap<QString, QString> > &value);

    void clearParam();

    QString getFileName() const;
    void setFileName(const QString &value);


private:
    QMap<QString, QString> info;
    QMap<QString, QMap<QString, QString>> param;
    QString desc;
    QString fileName;
};

#endif // ALGORITHMCOMP_H
