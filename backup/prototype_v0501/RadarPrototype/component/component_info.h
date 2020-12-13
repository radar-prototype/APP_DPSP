#ifndef COMPONENT_INFO_H
#define COMPONENT_INFO_H
#include <QWidget>
#include "algorithmcomp.h"
class Component_info : public QWidget
{
    Q_OBJECT
public:
    explicit Component_info(QWidget *parent = nullptr);
    //shao: 加载组件信息
    void setInfoWindow(QWidget *parentWidget,AlgorithmComp* ac);

signals:

public slots:
};

#endif // COMPONENT_INFO_H
