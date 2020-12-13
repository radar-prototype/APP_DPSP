#ifndef MYGLWIDGWT_H
#define MYGLWIDGWT_H

#include <QWidget>

namespace Ui {
class myGLWidgwt;
}

class myGLWidgwt : public QWidget
{
    Q_OBJECT

public:
    explicit myGLWidgwt(QWidget *parent = nullptr);
    ~myGLWidgwt();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    Ui::myGLWidgwt *ui;
    int xRot;
    int yRot;
    int zRot;
};

#endif // MYGLWIDGWT_H
