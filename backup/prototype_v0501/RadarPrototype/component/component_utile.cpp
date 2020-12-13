#include "component_utile.h"
#include "utils.h"
Component_utile::Component_utile()
{

}
QList<QString> Component_utile::read_file_get_name_list()
{
    // 读取文件夹下所有的文件
    QList<QString> nameList;
    QString m_sProjectPath = QDir::currentPath() + "/algoXml/"; /*文件夹全路径名*/
//    qDebug() << "存本地组件库的路径:" << m_sProjectPath;
    QDir dir = Utils::openDirOrCreate(m_sProjectPath);
    dir.setFilter(QDir::Files); /*设置dir的过滤模式,表示只遍历本文件夹内的文件*/
    QStringList filterList;
    filterList << "*.xml";
    // for循环读取有点慢
    foreach(QFileInfo fileInfo, dir.entryInfoList(filterList))
    {
        QString absolute_file_path = fileInfo.absoluteFilePath();
        QString fileName = fileInfo.baseName();/*获取文件后名(不带后缀的文件名)*/
        if(!nameList.contains(fileName))
        {
            nameList.append(fileName);
//            qDebug() << "component_utile加入组件名:" << fileName;
        }
        else
        {
            qDebug() << "加载组件时有重复的文件名存在，文件名： " << fileName;
//            emit send2AppOutput(QStringLiteral("[警告]： 加载组件时有重复的文件名存在，文件名： ") + fileName, 0);
        }
    }
    return nameList;
}
