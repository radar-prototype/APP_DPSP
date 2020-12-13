#ifndef REALTIMECURVE_H
#define REALTIMECURVE_H

#include <QMainWindow>

namespace Ui {
class RealTimeCurve;
}
/**
 * @brief The RealTimeCurve class （没用到）随机生成波形
 */
class RealTimeCurve : public QMainWindow
{
    Q_OBJECT

public:
    explicit RealTimeCurve(QString p, QWidget *parent = nullptr);
    ~RealTimeCurve();
    void readFile(QString filePath);

private:
    Ui::RealTimeCurve *ui;
    int p, t;   //p represent the x coordinate of the start point of painting, t represent the  x coordinate of time
    QTimer *timer;
    QPoint *point;
    QPainterPath *path;
    void virtual paintEvent(QPaintEvent *event);
    QVector<double> realArray;
    int index=0;
    QString fp;

private slots:
    void timerUpdate();
    void startPainting();
    void stopPainting();
};

#endif // REALTIMECURVE_H
