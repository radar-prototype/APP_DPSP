#ifndef RADARPROPERTY_H
#define RADARPROPERTY_H
/**
* @projectName   prototype_v0719
* @brief         雷达属性窗口类。
* @author        Antrn
* @date          2019-08-12
*/
#include <QDialog>
#include <QWidget>

namespace Ui {
class RadarProperty;
}

class RadarProperty : public QDialog
{
    Q_OBJECT

public:
    explicit RadarProperty(QWidget *parent = nullptr);
    ~RadarProperty();

private:
    Ui::RadarProperty *ui;
};

#endif // RADARPROPERTY_H
