#ifndef A_SHOW_H
#define A_SHOW_H

#include <QWidget>

namespace Ui {
class A_show;
}
/**
 * @brief The A_show class  A显内部显示主控件（
 */
class A_show : public QWidget
{
    Q_OBJECT

public:
    explicit A_show(QWidget *parent = nullptr);
    ~A_show();

public slots:
    void paintEvent(QPaintEvent *);

private:
    Ui::A_show *ui;
    int p, t;   //p represent the x coordinate of the start point of painting, t represent the  x coordinate of time
    QTimer *timer;
    QPoint *point;
    QPainterPath *path;

private slots:
    void timerUpdate();
    void startPainting();
    void stopPainting();

};

#endif // A_SHOW_H
