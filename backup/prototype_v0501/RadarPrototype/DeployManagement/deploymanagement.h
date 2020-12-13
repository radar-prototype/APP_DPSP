#ifndef DEPLOYMANAGEMENT_H
#define DEPLOYMANAGEMENT_H

#include <QWidget>
#include <QHBoxLayout>
#include "clusterstatedialog.h"

class DeployManagement : public QWidget
{
    Q_OBJECT
public:
    explicit DeployManagement(QWidget *parent = nullptr);
    ~DeployManagement();
signals:

public slots:
    void changeClusterState();
private:
    QHBoxLayout *mainLayout;
    ClusterStateDialog *clusterStateDialog;
};

#endif // DEPLOYMANAGEMENT_H
