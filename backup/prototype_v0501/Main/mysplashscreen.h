#ifndef MYSPLASHSCREEN_H
#define MYSPLASHSCREEN_H

/**
* @projectName   prototype_v0719
* @brief         splash screen顾名思义，启动程序的启动画面。
* @author        Antrn
* @date          2019-08-12
*/
#include <QProgressBar>
#include <QSplashScreen>

class MySplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    MySplashScreen(QPixmap &pixmap, int time);
    ~MySplashScreen();
    void setProgress();
    void generateAscendRandomNumber();
    QWidget *w;

private:
    QProgressBar *progressBar;
    //随机数列表
    QList<int> numberList;
    //启动时停留时间
    int elapseTime;
//    QWidget *w;

private slots:
    void updateProgress();
    void showProgressMessage(const QString &msg);
};

#endif // MYSPLASHSCREEN_H
