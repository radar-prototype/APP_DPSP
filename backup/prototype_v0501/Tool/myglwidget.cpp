//#include "myglwidget.h"
//#include <QOpenGLWidget>
//#include <QMouseEvent>
///**
//* @projectName   prototype_v0719
//* @brief         使用OpenGL渲染的3D金字塔棱锥模型。以后如果用到可以借鉴或者删除。
//* @author        Antrn
//* @date          2019-08-12
//*/
//myGLWidget::myGLWidget(QWidget *parent):QOpenGLWidget(parent)
//{
//    xRot = 0;
//    yRot = 0;
//    zRot = 0;

//}
//static void qNormalizeAngle(int &angle)
//{
//    while (angle < 0)
//        angle += 360 * 16;
//    while (angle > 360)
//        angle -= 360 * 16;
//}


//void myGLWidget::setXRotation(int angle)
//{
//    qNormalizeAngle(angle);
//    if (angle != xRot) {
//        xRot = angle;
//        emit xRotationChanged(angle);
//        update();
//    }
//}

//void myGLWidget::setYRotation(int angle)
//{
//    qNormalizeAngle(angle);
//    if (angle != yRot) {
//        yRot = angle;
//        emit yRotationChanged(angle);
//        update();
//    }
//}

//void myGLWidget::setZRotation(int angle)
//{
//    qNormalizeAngle(angle);
//    if (angle != zRot) {
//        zRot = angle;
//        emit zRotationChanged(angle);
//        update();
//    }
//}

//void myGLWidget::initializeGL()
//{
//    glClearColor(0,0,0,0);

//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);

//    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
//}

//void myGLWidget::paintGL()
//{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//    glTranslatef(0.0, 0.0, -10.0);
//    glRotatef(static_cast<GLfloat>(xRot / 16.0), 1.0, 0.0, 0.0);
//    glRotatef(static_cast<GLfloat>(yRot / 16.0), 0.0, 1.0, 0.0);
//    glRotatef(static_cast<GLfloat>(zRot / 16.0), 0.0, 0.0, 1.0);
//    draw();
//}

//void myGLWidget::resizeGL(int width, int height)
//{
//    int side = qMin(width, height);
//        glViewport((width - side) / 2, (height - side) / 2, side, side);

//        glMatrixMode(GL_PROJECTION);
//        glLoadIdentity();
//    #ifdef QT_OPENGL_ES_1
//        glOrthof(-2, +2, -2, +2, 1.0, 15.0);
//    #else
//        glOrtho(-2, +2, -2, +2, 1.0, 15.0);
//    #endif
//        glMatrixMode(GL_MODELVIEW);
//}

//QSize myGLWidget::minimumSizeHint() const
//{
//    return QSize(50, 50);
//}

//QSize myGLWidget::sizeHint() const
//{
//    return QSize(400, 400);
//}

//void myGLWidget::mousePressEvent(QMouseEvent *event)
//{
//    lastPos = event->pos();
//}

//void myGLWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    int dx = event->x() - lastPos.x();
//    int dy = event->y() - lastPos.y();

//    if (event->buttons() & Qt::LeftButton) {
//        setXRotation(xRot + 8 * dy);
//        setYRotation(yRot + 8 * dx);
//    } else if (event->buttons() & Qt::RightButton) {
//        setXRotation(xRot + 8 * dy);
//        setZRotation(zRot + 8 * dx);
//    }

//    lastPos = event->pos();
//}

//void myGLWidget::draw()
//{
//    glColor3b(255,0,0);
//    glBegin(GL_QUADS);
//        glNormal3f(0,0,-1);
//        glVertex3f(-1,-1,0);
//        glVertex3f(-1,1,0);
//        glVertex3f(1,1,0);
//        glVertex3f(1,-1,0);

//    glEnd();
//    glBegin(GL_TRIANGLES);
//        glNormal3f(0,-1,0.707);
//        glVertex3f(-1,-1,0);
//        glVertex3f(1,-1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();
//    glBegin(GL_TRIANGLES);
//        glNormal3f(1,0, 0.707);
//        glVertex3f(1,-1,0);
//        glVertex3f(1,1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();
//    glBegin(GL_TRIANGLES);
//        glNormal3f(0,1,0.707);
//        glVertex3f(1,1,0);
//        glVertex3f(-1,1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();
//    glBegin(GL_TRIANGLES);
//        glNormal3f(-1,0,0.707);
//        glVertex3f(-1,1,0);
//        glVertex3f(-1,-1,0);
//        glVertex3f(0,0,1.2);
//    glEnd();
//}
