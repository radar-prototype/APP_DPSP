#include "clusterstatedialog.h"
#include <QPixmap>
#include <QLayout>
#include <QPainter>
#include <QLabel>
#include <QtDebug>
#include <QtXml/QDomDocument>
#include "expandnodedialog.h"
#include "requestMsg.h"
#include "Tool/utils.h"

ClusterStateDialog::ClusterStateDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("集群状态显示");
    setAttribute(Qt::WA_AlwaysShowToolTips);
    resize(QSize(800, 600));
    QImage *img = new QImage();
    //获取节点
    ClusterUtils::initClusterState();
    nodes = ClusterUtils::getNodeList();

    //initState();
    //生成几个节点
    //nodes.append(Node("127.0.0.1",true));
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addStretch();
    //显示所有节点Label
    for(auto node : nodes)
    {
        if(node.available)
        {
            img->load(":/img/availableNode.png");
        }
        else
        {
            img->load(":/img/unavailableNode.png");
        }
        QLabel *label = new QLabel(this);
        label->setPixmap(QPixmap::fromImage(*img).scaled(60, 60));
        label->setToolTip(QString().sprintf("ip地址：%s\n", node.nodeIP.toUtf8().data()));
        topLayout->addWidget(label, 0, Qt::AlignCenter);
        topLayout->addStretch();
    }

    computerLabel = new QLabel(this);
    img->load(":/img/computer.png");
    computerLabel->setPixmap(QPixmap::fromImage(*img).scaled(100, 100));
    computerLabel->setToolTip(QString().sprintf("节点总数：%d", nodes.size()));
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(computerLabel, 0, Qt::AlignCenter);
    delete img;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    update();
    creatMenuOnRightClick();

}

ClusterStateDialog::~ClusterStateDialog()
{

}

void ClusterStateDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    int num = nodes.size();
    if(num == 0)
        return;
    if(num == 1)
    {
        painter.drawLine(width() / 2, height() / 4 + 30, width() / 2, 3 * height() / 4 - 30);
        return;
    }
    //节点之间间隔
    int space = (width() - num * 60) / (num + 1);
    int w;
    if(num % 2 == 0)
        w = num * 30 - 30 + (num / 2 - 1) * space + space / 2;
    else
    {
        w = num / 2 * (space + 60);
    }
    painter.drawLine(width() / 2 - w, 3 * height() / 4, width() / 2 + w, 3 * height() / 4);
    for(int i = 0; i < num; ++i)
    {
        painter.drawLine(width() / 2 - w + (space + 60) * i, height() / 4 + 30, width() / 2 - w + (space + 60) * i, 3 * height() / 4);
    }
}

void ClusterStateDialog::creatMenuOnRightClick()
{
    //初始化动作
    QAction *resourceMoniterAction = new QAction("资源监视器", this);
    QAction *expandNodeAction = new QAction("扩展节点", this);
    resourceMoniterAction->setIcon(QIcon(":/img/resourceMonitorIcon"));
    QMenu *menu = new QMenu(this);

    connect(resourceMoniterAction, &QAction::triggered, [ = ]()
    {
        ResourceMonitor *resourceMonitor = new ResourceMonitor(this);
//        resourceMonitor->show();
        qDebug() << "resource monitor";
    });
    connect(expandNodeAction, &QAction::triggered, [ = ]()
    {
        ExpandNodeDialog *expandNode = new ExpandNodeDialog(this);
        connect(expandNode, &ExpandNodeDialog::nodeListChanged, [ = ]()
        {
            nodes = ClusterUtils::getNodeList();
            update();
            emit changeState();
        });
        expandNode->show();
    });
    //创建菜单
    menu->addAction(resourceMoniterAction);
    menu->addAction(expandNodeAction);
    computerLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(computerLabel, &QLabel::customContextMenuRequested, [ = ](const QPoint & pos)
    {
        qDebug() << pos;
        menu->exec(QCursor::pos());
    });
}

void ClusterStateDialog::initState()
{
    //读取rpc.xml文件
    QDomDocument doc;
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
        // Antrn： TODO
        toPath << QString("./target/");
        shellPkg << QString("FDPC2.sh");
        if(Utils::generateXml4RPC(rpcfp, rpcfn, serviceIp, fromPath, toPath, shellPkg))
            rpcXml.open(QIODevice::ReadOnly);
    }
    if(!doc.setContent(&rpcXml))
        rpcXml.close();

    QDomElement root = doc.documentElement();
    //读取第一个node节点
    QDomNode node = doc.documentElement().firstChild();
    while(!node.isNull())
    {
        if(node.firstChild().toElement().tagName() == "server-IP")
            nodes.append(Node(node.firstChild().toElement().text(), true));
        node = node.nextSibling();
    }
    rpcXml.close();
}
