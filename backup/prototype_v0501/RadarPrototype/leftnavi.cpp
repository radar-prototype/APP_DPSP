#include "leftnavi.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QDebug>
#include <QToolButton>

LeftNavi::LeftNavi(QFrame *parent) :
    QFrame(parent)
{
    naviToolBar = new QToolBar(this);
    naviToolBar->setObjectName("naviToolBar");

    // 导航栏底部颜色
    naviToolBar->setStyleSheet(QString("#naviToolBar { border-bottom: 1px solid %1;}").
                               arg(qApp->palette().color(QPalette::Dark).name()));
    // 设置导航栏/工具栏
    fillToolBar();

///Begin places Tree View
    QLabel *placeHeadLabel = new QLabel(this);
    placeHeadLabel->setText("Places");

    QToolButton *placeHead = new QToolButton(this);
    placeHead->setIcon(QIcon(":/img/add.png"));
    placeHead->setFocusPolicy(Qt::NoFocus);
    placeHead->setMaximumSize(16, 16);
    placeHead->setAutoRaise(true);

    QHBoxLayout *placePanelLayout = new QHBoxLayout();
    placePanelLayout->setMargin(1);
    placePanelLayout->addSpacing(2);
    placePanelLayout->addWidget(placeHeadLabel, 1);
    placePanelLayout->addWidget(placeHead);

    // 包围place和后面toolbutton的widget
    QWidget *placePanel = new QWidget(this);
    placePanel->setObjectName("placePanel");
    placePanel->setLayout(placePanelLayout);
    placePanel->setStyleSheet(QString("#placePanel { border-bottom: 1px solid %1;}").
                              arg(qApp->palette().color(QPalette::Dark).name()));

    lw = new QListWidget(this);
    lw->addItem("hello");
    QVBoxLayout *placeLayout = new QVBoxLayout();
    placeLayout->setMargin(0);
    placeLayout->setSpacing(0);
    placeLayout->addWidget(placePanel);
    placeLayout->addWidget(lw, 1);

    // 包括上面的placePanel和树
    QWidget *placeWidget = new QWidget(this);
    placeWidget->setLayout(placeLayout);
    placeWidget->setStyleSheet("QWidget { border: none; padding: 0px;}");
//End place Tree

//Begin Network Tree
    QLabel *netHeadLabel = new QLabel(this);
    netHeadLabel->setText("Networks");
    netHead = new QToolButton(this);
    netHead->setIcon(QIcon(":/img/delete.png"));
    netHead->setFocusPolicy(Qt::NoFocus);
    netHead->setMaximumSize(16, 16);
    netHead->setAutoRaise(true);

    QHBoxLayout *netPanelLayout = new QHBoxLayout();
    netPanelLayout->setMargin(1);
    netPanelLayout->addSpacing(2);
    netPanelLayout->addWidget(netHeadLabel, 1);
    netPanelLayout->addWidget(netHead);
    QWidget *netPanel = new QWidget(this);
    netPanel->setObjectName("netPanel");
    netPanel->setLayout(netPanelLayout);
    netPanel->setStyleSheet(QString("#netPanel { border-bottom: 1px solid %1;}").
                            arg(qApp->palette().color(QPalette::Dark).name()));

    lw2 = new QListWidget(this);
    lw2->addItem("你好");
    QVBoxLayout *netLayout = new QVBoxLayout();
    netLayout->setMargin(0);
    netLayout->setSpacing(0);
    netLayout->addWidget(netPanel);
    netLayout->addWidget(lw2, 1);
    QWidget *netWidget = new QWidget(this);
    netWidget->setLayout(netLayout);
//End Network Tree

    vSplit = new QSplitter(Qt::Vertical);
    vSplit->setFrameStyle(QFrame::NoFrame);
    vSplit->setChildrenCollapsible(false);
    vSplit->setHandleWidth(1);

    vSplit->setStyleSheet(
        QString("QSplitter::handle {background: qlineargradient("
                "x1: 0, y1: 0, x2: 0, y2: 1,"
                "stop: 0 %1, stop: 0.07 %2);}").
        arg(qApp->palette().window().color().name()).
        arg(qApp->palette().color(QPalette::Dark).name()));
//            QString("QSplitter::handle {background-color:blue;}"));
    vSplit->addWidget(placeWidget);
    vSplit->addWidget(netWidget);

    QVBoxLayout *navilay = new QVBoxLayout(this);
    navilay->setMargin(0);
    navilay->setSpacing(0);
    navilay->addWidget(naviToolBar);
    navilay->addWidget(vSplit);
    setLayout(navilay);

    connect(netHead, SIGNAL(clicked()), this, SLOT(showNetTree()));
    netHeadLabel->installEventFilter(this);
}


LeftNavi::~LeftNavi()
{

}

void LeftNavi::fillToolBar()
{
    QAction *actAdd = new QAction(this);
    actAdd->setText("Add");
    actAdd->setIcon(QIcon(":/image/naviBar/add.png"));
    naviToolBar->addAction(actAdd);
    naviToolBar->addSeparator();

    QAction *actFilter = new QAction(this);
    actFilter->setText("Filter");
    actFilter->setIcon(QIcon(":/image/naviBar/filter.png"));
    naviToolBar->addAction(actFilter);
    naviToolBar->addSeparator();

    QAction *actUpdate = new QAction(this);
    actUpdate->setText("Update");
    actUpdate->setIcon(QIcon(":/image/naviBar/update.png"));
    naviToolBar->addAction(actUpdate);
    naviToolBar->addSeparator();

    actSearch = new QAction(this);
    actSearch->setText("Search");
    actSearch->setIcon(QIcon(":/image/naviBar/find.png"));
    naviToolBar->addAction(actSearch);
    actSearch->setCheckable(true);
    actSearch->setChecked(false);
    naviToolBar->addSeparator();

    connect(actSearch, SIGNAL(triggered()), this, SLOT(slotShowFind()));
}

void LeftNavi::showNetTree()
{
    if (lw->isHidden())
    {
        netHead->setIcon(QIcon(":/image/naviBar/netHide.png"));
        netHead->setToolTip(tr("Hide Network"));
        lw->show();
        vSplit->restoreState(vSplitState);
    }
    else
    {
        vSplitState = vSplit->saveState();
        netHead->setIcon(QIcon(":/image/naviBar/netShow.png"));
        netHead->setToolTip(tr("Show Network"));
        lw->hide();
        QList <int> sizes;
        qDebug() << "height:" << height();
        sizes << height() << 20;
        vSplit->setSizes(sizes);
    }
}
