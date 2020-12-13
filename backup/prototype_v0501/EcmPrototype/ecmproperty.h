#ifndef ECMPROPERTY_H
#define ECMPROPERTY_H

#include <QDialog>

namespace Ui {
class EcmProperty;
}

class EcmProperty : public QDialog
{
    Q_OBJECT

public:
    explicit EcmProperty(QWidget *parent = nullptr);
    ~EcmProperty();

private:
    Ui::EcmProperty *ui;
};

#endif // ECMPROPERTY_H
