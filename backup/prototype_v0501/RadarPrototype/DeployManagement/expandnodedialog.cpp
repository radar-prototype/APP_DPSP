#include "expandnodedialog.h"
#include <QLayout>
#include <QMessageBox>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDebug>
#include "Tool/utils.h"
#include "clusterutils.h"

ExpandNodeDialog::ExpandNodeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("扩展节点");
    this->setAttribute(Qt::WA_DeleteOnClose);
    //layout
    QHBoxLayout *inputLayout = new QHBoxLayout;
    label = new QLabel(this);
    label->setText(tr("请输入节点IP地址："));
    ipEdit = new QLineEdit(this);
    inputLayout->addWidget(label);
    inputLayout->addWidget(ipEdit);

    QHBoxLayout *pushButtonLayout = new QHBoxLayout;
    addNodePushButton = new QPushButton("增加节点",this);
    deleteNodePushButton = new QPushButton("删除节点",this);
    pushButtonLayout->addWidget(addNodePushButton);
    pushButtonLayout->addWidget(deleteNodePushButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(pushButtonLayout);

    //connect
    connect(addNodePushButton,&QPushButton::clicked,[=](){
        input = ipEdit->text();
        switch(ClusterUtils::addNode(input))
        {
        case 0:
            QMessageBox::about(this, "INFORMATION", "SUCCESS");

            emit nodeListChanged();
            //this->close();
            break;
        case 1:
            QMessageBox::warning(this, "WARNING!", "ipv4 address invalid！");
            qDebug() << "ipv4 address invalid!";
            break;
        case 2:
            QMessageBox::warning(this, "WARNING!", "node exist！");
            qDebug() << "exist!";
            break;
        default:
            QMessageBox::warning(this, "WARNING!", "fail to change node！");
            qDebug() << "fail to change node";
        }

    });
    connect(deleteNodePushButton,&QPushButton::clicked,[=](){
        input = ipEdit->text();
        switch(ClusterUtils::deleteNode(input))
        {
        case 0:
            QMessageBox::about(this, "INFORMATION", "SUCCESS");
            emit nodeListChanged();
            //this->close();
            break;
        case 1:
            QMessageBox::warning(this, "WARNING!", "ipv4 address invalid！");
            qDebug() << "ipv4 address invalid!";
            break;
        case 2:
            QMessageBox::warning(this, "WARNING!", "node not exist！");
            qDebug() << "not exist!";
            break;
        default:
            QMessageBox::warning(this, "WARNING!", "fail to change node！");
            qDebug() << "fail to change node";
        }
    });

}

ExpandNodeDialog::~ExpandNodeDialog()
{

}
