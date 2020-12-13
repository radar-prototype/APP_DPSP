#ifndef DATAPLAYBACKWIDGET_H
#define DATAPLAYBACKWIDGET_H

#include <QWidget>

namespace Ui {
class DataPlayBackWidget;
}

/**
 * @brief The DataPlayBackWidget class 数据回放窗口（总体仿真窗口 ）
 */
class DataPlayBackWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataPlayBackWidget(QString window_name, QWidget *parent = nullptr);
    ~DataPlayBackWidget();

private slots:
    void on_pushButton_radar_add_clicked();

    void on_pushButton_radar_del_clicked();

    void on_pushButton_set_toggled(bool checked);

    void on_pushButton_apply_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_yes_clicked();

    void on_pushButton_load_clicked();

private:
    Ui::DataPlayBackWidget *ui;
    // 窗口名称
    QString winName;
};

#endif // DATAPLAYBACKWIDGET_H
