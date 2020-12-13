#ifndef CLUSTERSTATEDIALOG_H
#define CLUSTERSTATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include "resourcemonitor.h"
/*
struct Node
{
    QString type;
    QString specs;
    bool available;
    Node(QString t,QString s,bool a) : type(t),specs(s),available(a) {}
};
*/
class ClusterStateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClusterStateDialog(QWidget *parent = nullptr);
    ~ClusterStateDialog();
    void paintEvent(QPaintEvent *);
    void creatMenuOnRightClick();
    void initState();
signals:
    void changeState();
private:
    QList<Node> nodes;
    QLabel *computerLabel;

};

#endif // CLUSTERSTATEDIALOG_H
