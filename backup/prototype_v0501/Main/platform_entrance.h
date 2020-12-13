#ifndef PLATFORM_ENTRANCE_H
#define PLATFORM_ENTRANCE_H

#include <QDialog>

namespace Ui {
class Platform_entrance;
}
/**
 * @brief The Platform_entrance class 平台入口（包含）
 */
class Platform_entrance : public QDialog
{
    Q_OBJECT

public:
    explicit Platform_entrance(QWidget *parent = nullptr);
    ~Platform_entrance();

private slots:
    void on_pushButton_radar_clicked(bool checked);

private:
    Ui::Platform_entrance *ui;
};

#endif // PLATFORM_ENTRANCE_H
