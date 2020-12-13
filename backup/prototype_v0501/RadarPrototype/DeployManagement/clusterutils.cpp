#include "clusterutils.h"
#include "requestMsg.h"
#include <QString>
#include <QtXml/QDomDocument>
#include <QtDebug>
#include "Tool/utils.h"

// fanzhouyang

int ClusterUtils::nodeNumber = 0;

ClusterUtils::ClusterUtils()
{
}

void ClusterUtils::initClusterState()
{
    //读取rpc.xml文件
    QFile rpcXml("./rpc.xml");
    if (!rpcXml.open(QIODevice::ReadOnly))
    {
        qDebug() << "rpc.xml不存在，正在创建";
        QString rpcfp = "./";//
        //QCoreApplication::applicationDirPath();
        QString rpcfn = "rpc.xml";
        QList<QString> serviceIp, fromPath, toPath, shellPkg;
        serviceIp << QString("127.0.0.1");
        fromPath << QString("../Main/");
        toPath << QString("/home/wangyuan/OpenDDS-3.13.3/DevGuideExamples/DCPS/");
        shellPkg << QString("FDPC2.sh");
        Utils::generateXml4RPC(rpcfp, rpcfn, serviceIp, fromPath, toPath, shellPkg);
        return;
    }
    rpcXml.close();

    nodeNumber = getNodeList().size();
}

QList<Node> ClusterUtils::getNodeList()
{
    //读取rpc.xml文件
    int index = 0;
    QList<Node> nodes;
    QDomDocument doc;
    QFile rpcXml("./rpc.xml");

    if(!doc.setContent(&rpcXml))
        rpcXml.close();

    QDomElement root = doc.documentElement();
    //读取第一个node节点
    QDomNode node = doc.documentElement().firstChild();
    while(!node.isNull())
    {
        if(node.firstChild().toElement().tagName() == "server-IP")
            nodes.append(Node(node.firstChild().toElement().text(), nodeIsAvailable(index++)));
        node = node.nextSibling();
    }
    rpcXml.close();
    return nodes;
}

int ClusterUtils::getNodeNumber()
{
    return nodeNumber;
}

QList<int> ClusterUtils::parseInformationFromRpc(int i)
{
    return {};
}

bool ClusterUtils::nodeIsAvailable(int i)
{
    //nodeNumber = 1;
    char *buffer[10];
    for(int i = 0; i < 10; ++i)
    {
        buffer[i] = static_cast<char *>(malloc(sizeof(char) * 100));
    }

    // NOTE 需要配置链接库
    //getStatus(buffer);
    //QString status = buffer[0];

    QStringList status = QString(buffer[i]).split("\n");
    status.removeLast();

    for(int i = 0; i < 10; ++i)
    {
        delete buffer[i];
    }

    return !status.empty();
}

int ClusterUtils::addNode(QString newIp)
{
    if(!isIpv4Format(newIp))
        return 1;
    QList<Node> nodes = getNodeList();
    QList<QString> nodeIp, fromPath, toPath, shellPkg;
    QString rpcfp = QDir::currentPath() + "/";
    QString rpcfn = "rpc.xml";
    for(auto node : nodes)
    {
        nodeIp << node.nodeIP;
        fromPath << QString("../Main/");
        toPath << QString("/home/wangyuan/OpenDDS-3.13.3/DevGuideExamples/DCPS/");
        shellPkg << QString("FDPC2.sh");
    }
    int pos = nodeIp.indexOf(newIp);
    if(pos != -1)
        return 2;
    nodeIp << newIp;
    fromPath << QString("../Main/");
    toPath << QString("/home/wangyuan/OpenDDS-3.13.3/DevGuideExamples/DCPS/");
    shellPkg << QString("FDPC2.sh");
    //write
    /*
    QDomDocument doc;
    QFile rpcXml("./rpc.xml");
    if(!rpcXml.open(QIODevice::WriteOnly))
        return 3;
        */
    if(Utils::generateXml4RPC(rpcfp, rpcfn, nodeIp, fromPath, toPath, shellPkg))
    {
        //rpcXml.close();
        ++nodeNumber;
        return 0;
    }
    return 0;
}

int ClusterUtils::deleteNode(QString newIp)
{
    if(!isIpv4Format(newIp))
        return 1;
    QList<Node> nodes = getNodeList();
    QList<QString> nodeIp, fromPath, toPath, shellPkg;
    QString rpcfp = QDir::currentPath() + "/";
    QString rpcfn = "rpc.xml";
    for(auto node : nodes)
    {
        nodeIp << node.nodeIP;
        fromPath << QString("../Main/");
        toPath << QString("/home/wangyuan/OpenDDS-3.13.3/DevGuideExamples/DCPS/");
        shellPkg << QString("FDPC2.sh");
    }
    int pos = nodeIp.indexOf(newIp);
    if(pos == -1)
        return 2;
    nodeIp.removeAt(pos);
    fromPath.removeAt(pos);
    toPath.removeAt(pos);
    shellPkg.removeAt(pos);
    //write
    /*
    QDomDocument doc;
    QFile rpcXml("./rpc.xml");
    if(!rpcXml.open(QIODevice::WriteOnly))
        return 3;
        */
    if(Utils::generateXml4RPC(rpcfp, rpcfn, nodeIp, fromPath, toPath, shellPkg))
    {
        //rpcXml.close();
        --nodeNumber;
        return 0;
    }
    return 0;
}

bool ClusterUtils::isIpv4Format(QString ip)
{

    if(ip.contains(QRegExp("[^0-9.]")))
        return false;
    QStringList ss = ip.split(".");
    if(ss.size() != 4)
        return false;
    if(ss[0].toInt() == 0)
        return false;
    for(int i = 0; i < 4; ++i)
    {
        if(ss[i].toInt() < 0 || ss[i].toInt() > 255)
        {
            return false;
        }
        if(ss[i].size() > 1 && ss[i].startsWith('0'))
            return false;
    }
    return true;
}
