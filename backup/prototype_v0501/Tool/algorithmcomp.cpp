#include "algorithmcomp.h"

AlgorithmComp::AlgorithmComp()
{

}

QMap<QString, QString> AlgorithmComp::getInfo() const
{
    return info;
}

void AlgorithmComp::setInfo(const QMap<QString, QString> &value)
{
    info = value;
}

QString AlgorithmComp::getDesc() const
{
    return desc;
}

void AlgorithmComp::setDesc(const QString &value)
{
    desc = value;
}

QMap<QString, QMap<QString, QString> > AlgorithmComp::getParam() const
{
    return param;
}

void AlgorithmComp::setParam(const QMap<QString, QMap<QString, QString> > &value)
{
    param = value;
}

void AlgorithmComp::clearParam()
{
    param.clear();
}

QString AlgorithmComp::getFileName() const
{
    return fileName;
}

void AlgorithmComp::setFileName(const QString &value)
{
    fileName = value;
}

