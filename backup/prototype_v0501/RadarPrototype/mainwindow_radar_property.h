#ifndef MAINWINDOW_RADAR_PROPERTY_H
#define MAINWINDOW_RADAR_PROPERTY_H
#include <QDockWidget>
#include "algorithmcomp.h"
#include "radarscene.h"
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "utils.h"
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QFormLayout>
namespace Ui
{
class mainwindow_radar_property;
}

/**
 * @brief The mainwindow_radar_property class 雷达页面右边的   显示的雷达属性的窗口
 */

class mainwindow_radar_property : public QDockWidget
{
    Q_OBJECT

signals:
    void compUpdata(AlgorithmComp ac2);//还没 用到

public:
    explicit mainwindow_radar_property(QWidget *parent = nullptr);
    ~mainwindow_radar_property();
    void create( AlgorithmComp ac, bool isReadonly, RadarScene* scene, QString Equip_id);

};
#endif // MAINWINDOW_RADAR_PROPERTY_H
