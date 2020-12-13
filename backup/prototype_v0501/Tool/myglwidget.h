//#ifndef MYGLWIDGET_H
//#define MYGLWIDGET_H
//#include <QOpenGLWidget>
///**
//* @projectName   prototype_v0719
//* @brief         使用OpenGL渲染的3D金字塔棱锥模型。以后如果用到可以借鉴或者删除。
//* @author        Antrn
//* @date          2019-08-12
//*/

////https://www.bogotobogo.com/Qt/Qt5_OpenGL_QGLWidget.php
//class myGLWidget : public QOpenGLWidget
//{
//    Q_OBJECT
//public:
//    explicit myGLWidget(QWidget *parent=nullptr);

//signals:
//    // signaling rotation from mouse movement
//    void xRotationChanged(int angle);
//    void yRotationChanged(int angle);
//    void zRotationChanged(int angle);

//public slots:
//    // slots for xyz-rotation slider
//    void setXRotation(int angle);
//    void setYRotation(int angle);
//    void setZRotation(int angle);

//protected:
//    void initializeGL();
//    void paintGL();
//    void resizeGL(int width, int height);

//    QSize minimumSizeHint() const;
//    QSize sizeHint() const;
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

//private:
//    void draw();
//    int xRot;
//    int yRot;
//    int zRot;
//    QPoint lastPos;
//};

//#endif // MYGLWIDGET_H
