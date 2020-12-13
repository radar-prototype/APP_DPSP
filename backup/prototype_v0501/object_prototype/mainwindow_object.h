#ifndef MAINWINDOW_OBJECT_H
#define MAINWINDOW_OBJECT_H

#include <QMainWindow>

namespace Ui {
class MainWindow_object;
}

class MainWindow_object : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_object(QWidget *parent = nullptr);
    ~MainWindow_object();

private:
    Ui::MainWindow_object *ui;
};

#endif // MAINWINDOW_OBJECT_H
