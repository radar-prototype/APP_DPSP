#ifndef COMPONENT_PROCESS_H
#define COMPONENT_PROCESS_H

#include <QObject>
#include "codewindow.h"
#include "algorithmcomp.h"
#include "diagramitem.h"
#include "RadarPrototype/component/component_info_dialog.h"

class Component_process : public QObject
{
    Q_OBJECT
public:
    explicit Component_process(QObject *parent = nullptr);
    void codeEdit(QListWidgetItem *item, QGraphicsItem*gitem, DiagramItem *ditem, AlgorithmComp* ac);
    //shao:子组件参数配置
    void parameter_configuration(AlgorithmComp algorithmComp, DiagramItem *gitem);
    //提供当前组件的参数的map
    QMap<QString, QMap<QString, QString>> get_parameter_value(AlgorithmComp* algorithmComp);
    QString get_parameter_init_code(AlgorithmComp* algorithmComp);
    QString get_ip_init_code(DiagramItem* diagramItem);
    QString get_h_code(AlgorithmComp* algorithmComp, DiagramItem * diagramItem, int thread_num);
    QString get_cpp_code(AlgorithmComp* algorithmComp, DiagramItem * diagramItem);
    QString get_txt(QString name, QString fullName);
    //shao:设置队列长度 源说应该要美国各组件不一样 留出来改
    QString get_queue_len();
signals:
    void edit_succeed(AlgorithmComp* ac);
public slots:

private:
    AlgorithmComp algorithmComp;
    Component_info_dialog* component_info_dialog;

};

#endif // COMPONENT_PROCESS_H
