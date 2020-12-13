#ifndef LEFT_SIDE_SWITCH_H
#define LEFT_SIDE_SWITCH_H

#include <QWidget>

#include <QToolButton>
#include <QVBoxLayout>
#include <qmap.h>
//#include <QMap> 一样
#include <QButtonGroup>

namespace Ui {
class Left_side_switch;
}

class Left_side_switch : public QWidget
{
    Q_OBJECT

public:
//    explicit left_side_switch(QWidget *parent = nullptr);//用这个cpp中 ui报错 为啥呢？？
        explicit Left_side_switch(QWidget *parent = 0);
    ~Left_side_switch();

    QToolButton* addTooButton(const QString &strName, const QString &strToolTip, const QIcon &icon);
    void setToolWidget(QToolButton *pToolBt, QWidget *pWidget);
    //按钮切换
    void switchover(int);
    //更换按钮对应的窗口
    void change_widget(int a,int b,QWidget *pWidget);
private:
    Ui::Left_side_switch *ui;

    void showWidget();

    QWidget *m_barWidget;
    QVBoxLayout *m_barLayout;
    QVector<QToolButton*> m_toolBts;

    QHBoxLayout *m_parentLayout;
    QWidget *m_blankWidget;
    QMap<QToolButton *, QWidget *> m_toolWidget;

    QToolButton *m_currentToolBt;

    QButtonGroup m_buttonGroup;

};

#endif // LEFT_SIDE_SWITCH_H
