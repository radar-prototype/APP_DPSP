#ifndef COMPENCAP_H
#define COMPENCAP_H

#include <QObject>

class CompEncap : public QObject
{
    Q_OBJECT
public:
    explicit CompEncap(QObject *parent = nullptr);

signals:

public slots:
};

#endif // COMPENCAP_H