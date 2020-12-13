#include <QLabel>
#include <QIcon>
#include "left_side_switch.h"

void test() {

    Left_side_switch* w=new  Left_side_switch();
    QToolButton *oneButton = w->addTooButton("", "回退", QIcon(":/img/hi.png"));
    w->addTooButton("11", "前进", QIcon(":/QSideBar/Resources/Toolbar_Redo_64px.ico"));

    QLabel labelPlayList("列表");
    w->setToolWidget(w->addTooButton("22", "列表", QIcon(":/img/hi.png")), &labelPlayList);
    QLabel labelCut("剪切");
    w->setToolWidget(w->addTooButton("33", "剪切", QIcon(":/img/hi.png")), &labelCut);
    QLabel labelSave("保存");
    w->setToolWidget(w->addTooButton("44", "保存",QIcon(":/img/hi.png")), &labelSave);
    QLabel labelReset("重置");
//    w.setToolWidget(w.addTooButton("55", "重置", QIcon(":/QSideBar/Resources/Toolbar_Replay_64px.ico")), &labelReset);
    qDebug("22");
    w->show();
    oneButton->click();
}
