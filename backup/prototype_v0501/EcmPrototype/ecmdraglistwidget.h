#ifndef ECMDRAGLISTWIDGET_H
#define ECMDRAGLISTWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QListWidget>
#include <QApplication>

/**
* @projectName   prototype_v0719
* @brief         整体工程项目左边的拖拽列表：展示电子对抗设备，以list的形式表现。
* @author        zhengyuming
* @date          2019-08-19
*/
class ECMDragListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ECMDragListWidget(QWidget *parent = nullptr);
    void addDragItem(QListWidgetItem*item);
    void addDragItem(const QString &label);
    static QString puzzleMimeType() {
        return QStringLiteral("image/x-module-piece");
        }

protected:
    //开始拖拽
    void startDrag(Qt::DropActions supportedActions);
    //从该部件中拖拽出去的操作.
    void mousePressEvent(QMouseEvent *event);


private:
    //记录拖拽的起点.
    QPoint m_dragPoint;
    //记录被拖拽的项.
    QListWidgetItem *m_dragItem;
    QListWidgetItem *addECMButton;

public slots:
};

#endif // ECMDRAGLISTWIDGET_H
