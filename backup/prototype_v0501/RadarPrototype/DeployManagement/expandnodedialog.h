#ifndef EXPANDNODEDIALOG_H
#define EXPANDNODEDIALOG_H
// fanzhouyang
// 扩展节点功能
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "clusterutils.h"

class ExpandNodeDialog : public QDialog
{
    Q_OBJECT

public:
    ExpandNodeDialog(QWidget *parent = nullptr);
    ~ExpandNodeDialog();
signals:
    void nodeListChanged();
private slots:
    //void addNode();
    //void deleteNode();
private:
    //int num;
    QString input;

    QLabel *label;
    QLineEdit *ipEdit;

    QPushButton *addNodePushButton;
    QPushButton *deleteNodePushButton;

    QList<Node> nodes;
    QList<QString> nodeIp;
    QString rpcfp = "./";
    QString rpcfn = "rpc.xml";
    QList<QString> fromPath,toPath,shellPkg;
};

#endif // EXPANDNODEDIALOG_H
