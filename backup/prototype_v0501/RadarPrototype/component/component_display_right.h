#ifndef COMPONENT_DISPLAY_RIGHT_H
#define COMPONENT_DISPLAY_RIGHT_H

#include <QObject>
#include "algorithmcomp.h"
#include "diagram_item.h"

class Component_display_right : public QObject
{
    Q_OBJECT
public:
    explicit Component_display_right(QObject *parent = nullptr);
    void displayCompenent(QWidget* parentWidget,AlgorithmComp* algorithmComp);

private:
    void getPara2Port(QString fileName, DiagramItem *item);

signals:

public slots:
};

#endif // COMPONENT_DISPLAY_RIGHT_H
