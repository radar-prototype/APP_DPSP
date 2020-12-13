#ifndef SIMDATAGENWIDGET_H
#define SIMDATAGENWIDGET_H

#include <QWidget>

namespace Ui
{
class SimDataGenWidget;
}
/**
 * @brief The SimDataGenWidget class  生成数据 窗口（点击工具栏 按钮 弹出）
 */
class SimDataGenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SimDataGenWidget(QString curr_eq, QWidget *parent = nullptr);
    ~SimDataGenWidget();

private slots:
//    void on_pushButton_select_clicked();

    void on_buttonBox_accepted();

    void on_pushButton_load_clicked();

    void on_buttonBox_rejected();
signals:
    void geneData(QString s);
private:
    Ui::SimDataGenWidget *ui;
    QString curr_equ; // 当前装备(雷达，对抗，目标)空间的名字
};

#endif // SIMDATAGENWIDGET_H
