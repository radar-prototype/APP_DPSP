#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QObject>

/**
 * @brief The myGraphicsView class 总体仿真的视图（和 scene 绑定的  一起完成控件显示等）
 */
class myGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit myGraphicsView(QWidget *parent = nullptr);
    // 平移速度
    void setTranslateSpeed(qreal speed);
    qreal translateSpeed() const;

    // 缩放的增量
    void setZoomDelta(qreal delta);
    qreal zoomDelta() const;

protected:
    // 上/下/左/右键向各个方向移动、加/减键进行缩放、空格/回车键旋转
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    // 平移
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    // 放大/缩小
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event);

public slots:
    void zoomIn();  // 放大
    void zoomOut();  // 缩小
    void zoom(float scaleFactor); // 缩放 - scaleFactor：缩放的比例因子
    void translate(QPointF delta);  // 平移

signals:
    void showTranslateBtn(bool tof);
private:
    Qt::MouseButton m_translateButton;  // 平移按钮
    qreal m_translateSpeed;  // 平移速度
    qreal m_zoomDelta;  // 缩放的增量
    bool m_bMouseTranslate;  // 平移标识
    QPoint m_lastMousePos;  // 鼠标最后按下的位置
    qreal m_scale;  // 缩放值
};

#endif // MYGRAPHICSVIEW_H
