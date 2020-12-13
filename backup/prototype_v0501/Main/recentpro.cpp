#include "recentpro.h"

#include <QEvent>
#include <QListWidgetItem>
#include <QPainter>
#include <QWidget>

RecentPro::RecentPro()
{
    initUi();
}

void RecentPro::initUi(){
    pro_head = new QWidget(this);
    pro_name = new QLabel;
    pro_desc = new QLabel;

    pro_head->setFixedSize(20,20);
    QPalette desc_color;
    desc_color.setColor(QPalette::Text, Qt::gray);
    pro_desc->setPalette(desc_color);

    pro_head->move(2,2);
    pro_name->move(28, 2);
    pro_desc->move(28, 16);

    pro_head->installEventFilter(this);
}

bool RecentPro::eventFilter(QObject *obj, QEvent *event){
    if(obj == pro_head){
        if(event->type() == QEvent::Paint){
            QPainter painter(pro_head);
            painter.drawPixmap(pro_head->rect(), QPixmap(head_path));
        }
    }
    return QWidget::eventFilter(obj, event);
}
