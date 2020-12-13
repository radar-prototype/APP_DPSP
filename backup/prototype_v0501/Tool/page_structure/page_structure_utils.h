#ifndef PAGE_STRUCTURE_UTILS_H
#define PAGE_STRUCTURE_UTILS_H

#include <QObject>

class Page_structure_utils : public QObject
{
    Q_OBJECT
public:
    explicit Page_structure_utils(QObject *parent = nullptr);
    enum window_type
    {
         main=0,radar,ecm,object, deploy
    };
signals:

public slots:
};

#endif // PAGE_STRUCTURE_UTILS_H
