#ifndef RECENTPRO_H
#define RECENTPRO_H

#include <QLabel>
#include <QObject>
#include <QListWidgetItem>

class RecentPro : public QListWidgetItem,QObject
{
    Q_OBJECT
public:
    RecentPro();
    void initUi();
    QWidget *pro_head;
    QLabel *pro_name;
    QLabel *pro_desc;
    QString head_path;
    QString pro_path;
    bool eventFilter(QObject *obj, QEvent *event);

signals:

public slots:
};

#endif // RECENTPRO_H
