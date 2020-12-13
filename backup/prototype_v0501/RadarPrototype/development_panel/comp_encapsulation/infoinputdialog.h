#ifndef INFOINPUTDIALOG_H
#define INFOINPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include "mainwindow_radar.h"

class InfoInputDialog : public QDialog
{
    Q_OBJECT
private:
    const int m_GroupCount;
    QLabel **m_Labels;
    QLineEdit **m_LineEdits;
    QPushButton *m_OKButton;
    QPushButton *m_CancelButton;
    QList<QString> nameList;
    MainWindow_Radar *mr;

public:
    InfoInputDialog(int count, QList<QString> nlist, MainWindow_Radar *m, QWidget *parent = nullptr);
    virtual ~InfoInputDialog();
    void SetLabelTexts(const QStringList &listText);
    void SetOneLabelText(int index, const QString &text);
    QString GetOneText(int index);
    QStringList GetAllTexts();
    //为了让多组输入框看起来整齐些，所以我固定了QLabel的宽度
    void SetLabelsWidth(int width);
    //使用正则表达式来限制输入的字符
    void SetLineEditRegExp(int index, QRegExp regExp);
    //请根据需要重写这两个函数
    virtual void accept();
    virtual void reject()
    {
        QDialog::reject();
    }
};

#endif // INFOINPUTDIALOG_H
