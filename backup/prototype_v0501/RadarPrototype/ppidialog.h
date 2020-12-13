#ifndef PPIDIALOG_H
#define PPIDIALOG_H

#include <QDialog>

namespace Ui {
class PPIDialog;
}
/**
 * @brief The PPIDialog class P显 （平面坐标显示器  2维 极坐标）
 */
class PPIDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PPIDialog(QWidget *parent = nullptr);
    ~PPIDialog();

private slots:


    void on_radioBtnShowOn_clicked(bool checked);

    void on_radioBtnShowOFF_clicked(bool checked);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_horizontalSlider_valueChanged(int value);
/*
    void on_radioButtonOffCenterOn_clicked(bool checked);

    void on_radioButtonOffCenterOff_clicked(bool checked);

    void on_radioButtonBShowOn_clicked(bool checked);

    void on_radioButtonBShowOff_clicked(bool checked);
*/
    void on_radioButtonNormal_clicked(bool checked);

    void on_radioButtonOffCenter_clicked(bool checked);

    void on_radioButtonBShow_clicked(bool checked);

private:
    Ui::PPIDialog *ui;
};

#endif // PPIDIALOG_H
