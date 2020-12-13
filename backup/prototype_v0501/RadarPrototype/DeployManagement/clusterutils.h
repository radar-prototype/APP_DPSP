#ifndef CLUSTERUTILS_H
#define CLUSTERUTILS_H
// fanzhouyang
// some cluster management tools

#include <QString>
#include <QList>

struct Node
{
    QString nodeIP;
    bool available;
    Node(QString s,bool b) : nodeIP(s),available(b) {}
};

class ClusterUtils
{
public:
    ClusterUtils();
    //create rpc.xml
    static void initClusterState();
    //get a list of Node
    static QList<Node> getNodeList();
    //get node number of whole cluster
    static int getNodeNumber();
    static QList<int> parseInformationFromRpc(int);
    //judge one node is available or not
    static bool nodeIsAvailable(int);
    static bool isIpv4Format(QString);

    static int addNode(QString);
    static int deleteNode(QString);

private:
    static int nodeNumber;
    static QList<Node> nodes;
};


#endif // CLUSTERUTILS_H
