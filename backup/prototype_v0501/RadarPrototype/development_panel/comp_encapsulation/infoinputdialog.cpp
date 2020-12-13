#include "infoinputdialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qvalidator.h>
#include <QDebug>
#include "utils.h"
#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow_radar.h"

// This is an auto-generated comment by Antrn.
/************************************
 * @author Antrn                    *
 * @date 2020-09-28 T 16:44:07      *
 * @description  用户输入对话窗口      *
 * @copyright (c)                   *
 ************************************/
InfoInputDialog::InfoInputDialog(int count, QList<QString> nlist, MainWindow_Radar *m, QWidget *parent)
    : QDialog(parent), m_GroupCount(count), nameList(nlist), mr(m)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *subLayout;
    m_Labels = new QLabel*[m_GroupCount];
    m_LineEdits = new QLineEdit*[m_GroupCount];
    //设计界面
    for(int i = 0; i < m_GroupCount; i++)
    {
//        qDebug() << i << "aaaaaaaaaaaaaaaaaaaaaaaaaaa";
        subLayout = new QHBoxLayout;
        m_LineEdits[i] = new QLineEdit(this);
        if(i == 0 || i == 1)
        {
            m_LineEdits[i]->setPlaceholderText("不能为空");
        }
        m_LineEdits[i]->setToolTip("请输入对应标签的内容");
//        connect(m_LineEdits[i], &QLineEdit::textChanged, this, [ = ]()
//        {
//            qDebug() << i << "个编辑框有改动";
//        });
        m_Labels[i] = new QLabel(this);
        subLayout->addWidget(m_Labels[i]);
        subLayout->addWidget(m_LineEdits[i]);
        layout->addLayout(subLayout);
    }
    // Antrn: DEL 删除默认填入的内容
    m_LineEdits[0]->setText("TEST");
    m_LineEdits[1]->setText("测试");
    m_LineEdits[2]->setText("test_func");

    m_OKButton = new QPushButton( tr("OK"), this );
    m_CancelButton = new QPushButton( tr("Cancel"), this );
    subLayout = new QHBoxLayout;
    subLayout->addStretch();
    subLayout->addWidget(m_OKButton);
    subLayout->addWidget(m_CancelButton);
    layout->addLayout(subLayout);
    setLayout(layout);
    this->setWindowTitle(tr("组件复用"));

    connect(m_OKButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(m_CancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

InfoInputDialog::~InfoInputDialog()
{
    delete m_LineEdits;
    delete m_Labels;
}

void InfoInputDialog::SetLabelTexts(const QStringList &listText)
{
    for(int i = 0; i < listText.size(); i++)
    {
        if( i >= m_GroupCount )
            break;
        m_Labels[i]->setText( listText.at(i) );
    }
}

void InfoInputDialog::SetOneLabelText(int index, const QString &text)
{
    m_Labels[index]->setText(text);
}

void InfoInputDialog::SetLabelsWidth(int width)
{
    for(int i = 0; i < m_GroupCount; i++)
        m_Labels[i]->setFixedWidth(width);
}

void InfoInputDialog::SetLineEditRegExp(int index, QRegExp regExp)
{
    QValidator *validator = new QRegExpValidator(regExp, this);
    m_LineEdits[index]->setValidator( validator );
}

void InfoInputDialog::accept()
{
//    qDebug() << m_LineEdits[0]->text() << "--hhhhh-" << m_LineEdits[1]->text();
    // Antrn: NOTE 使用isEmpty判断就不行
    if(m_LineEdits[0]->text().isEmpty() || m_LineEdits[1]->text().isEmpty() || m_LineEdits[0]->text().isNull() || m_LineEdits[1]->text().isNull())
    {
        Utils::alert(qApp->desktop()->rect(), "[警告]： 组件名不能为空！");
        // Antrn: 把这行注释，窗口就不会关掉了，但是对用户也不友好
        QDialog::reject();
    }
    // 检查组件库列表中是否已有该名字
    // Antrn: 血的教训，这里不加else就永远不会执行上一个if
    else if(nameList.contains(m_LineEdits[0]->text()))
    {
        qDebug() << "与现有的文件名: " << m_LineEdits[0]->text() << "重复，添加失败！";
        Utils::alert(qApp->desktop()->rect(), "[警告]： 已有重复名称存在，请重新新建组件！");
        QDialog::reject();
    }
    else
    {
        // 封装成子组件，加入组件库
        QString resultTip = mr->makeSelectedItems2OneComp(m_LineEdits[0]->text(), m_LineEdits[1]->text(), m_LineEdits[2]->text());
        Utils::alert(qApp->desktop()->rect(), resultTip);
        // Antrn:
        QDialog::accept();
    }
}

QString InfoInputDialog::GetOneText(int index)
{
    return m_LineEdits[index]->text();
}

QStringList InfoInputDialog::GetAllTexts()
{
    QStringList list;
    for(int i = 0; i < m_GroupCount; i++)
    {
        list.push_back( m_LineEdits[i]->text() );
    }
    return list;
}
