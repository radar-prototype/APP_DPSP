#ifndef OPENGL_H
#define OPENGL_H
/**
* @projectName   prototype_v0719
* @brief         OpenGL
* @author        Antrn
* @date          2019-08-12
*/
#include <QDialog>
#include <QWidget>

namespace Ui {
class OpenGL;
}

class OpenGL : public QDialog
{
    Q_OBJECT

public:
    explicit OpenGL(QWidget *parent = nullptr);
    ~OpenGL();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::OpenGL *ui;
};

#endif // OPENGL_H
