#include "deploymanagement.h"

DeployManagement::DeployManagement(QWidget *parent) : QWidget(parent)
{
    clusterStateDialog = new ClusterStateDialog;
    mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(clusterStateDialog);
    connect(clusterStateDialog,SIGNAL(changeState()),this,SLOT(changeClusterState()));
}

DeployManagement::~DeployManagement()
{
    delete clusterStateDialog;
}
void DeployManagement::changeClusterState()
{
    mainLayout->removeWidget(clusterStateDialog);
    delete clusterStateDialog;
    clusterStateDialog = new ClusterStateDialog;
    mainLayout->addWidget(clusterStateDialog);
    connect(clusterStateDialog,SIGNAL(changeState()),this,SLOT(changeClusterState()));
}
