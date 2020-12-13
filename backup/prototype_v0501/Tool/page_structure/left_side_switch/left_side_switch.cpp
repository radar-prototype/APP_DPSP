#include "left_side_switch.h"
#include "ui_left_side_switch.h"
#include "utils.h"
#include "left_utils.h"
Left_side_switch::Left_side_switch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Left_side_switch)
{
    ui->setupUi(this);
    m_parentLayout = new QHBoxLayout(this);
//    左上右下
    m_parentLayout->setContentsMargins(1, 0, 1, 0);
    m_barWidget = new QWidget(this);
//    m_barWidget->setFixedWidth(75);
    m_barLayout = new QVBoxLayout(m_barWidget);
    m_barLayout->setSpacing(0);
    m_barWidget->setObjectName("m_barWidget");
    m_barWidget->setFixedWidth(70);
    m_blankWidget = new QWidget(this);
    m_blankWidget->setObjectName("m_blankWidget");
    m_blankWidget->setVisible(false);

    m_parentLayout->addWidget(m_barWidget);
    m_parentLayout->addWidget(m_blankWidget);

    QSpacerItem *pItem = new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_barLayout->addSpacerItem(pItem);
    m_barLayout->setContentsMargins(0, 0, 0, 0);

    QFile file(":/qss/left_side_switch.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
}

Left_side_switch::~Left_side_switch()
{
    delete ui;
}

QToolButton *Left_side_switch::addTooButton(const QString &strName, const QString &strToolTip, const QIcon &icon)
{
    QToolButton *pToolBt = new QToolButton(m_barWidget);
    pToolBt->setText(strName);
    pToolBt->setFixedHeight(55);
    pToolBt->setFixedWidth(70);
    pToolBt->setToolTip(strToolTip);
    pToolBt->setIcon(icon);
    pToolBt->setIconSize(QSize(30, 30));
//    pToolBt->setStyleSheet(Utils::ReadQssFile(":/qss/left_side_switch.qss"));
    pToolBt->setCheckable(true);
    connect(pToolBt, &QToolButton::clicked, this, &Left_side_switch::showWidget);
    pToolBt->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_buttonGroup.addButton(pToolBt);
    m_toolBts.append(pToolBt);
    m_barLayout->insertWidget(m_barLayout->count() - 1, pToolBt);
    return pToolBt;
}

void Left_side_switch::setToolWidget(QToolButton *pToolBt, QWidget *pWidget)
{
    m_toolWidget[pToolBt] = pWidget;
    if (pWidget)
    {
        m_parentLayout->addWidget(pWidget);
        pWidget->setVisible(false);
    }
    else
    {
        qDebug() << "左边切换：222222222222222222";
    }
}

void Left_side_switch::showWidget()
{
    m_blankWidget->setVisible(false);
    // Antrn: 不加判断报错
    if(m_currentToolBt)
    {
        auto it = m_toolWidget[m_currentToolBt];
        if (it)
            it->setVisible(false);
    }

    QToolButton *pTool = qobject_cast<QToolButton*>(sender());
    if (pTool)
    {
        m_currentToolBt = pTool;

        auto it = m_toolWidget[pTool];
        if (it)
            it->setVisible(true);
        else
            m_blankWidget->setVisible(true);
    }
}

void Left_side_switch::switchover(int a)
{
    //万一切换的时候还要干啥事儿就提出来了
    m_toolBts[a]->click();
}
//a:哪个窗口，b:怎么切换，p：换成什么？
void Left_side_switch::change_widget(int a, int b, QWidget *pWidget)
{
    if(b == Left_utils::only_switch)
    {
        switchover(a);
    }
    else if (b == Left_utils::switch_update)
    {
//        qDebug() << "1111111111111111111111111111111";
        QToolButton* toolButton = m_toolBts[a];
        setToolWidget(toolButton, pWidget);
        switchover(a);
    }
    else
    {
        QToolButton* toolButton = m_toolBts[a];
        setToolWidget(toolButton, pWidget);
    }

}
