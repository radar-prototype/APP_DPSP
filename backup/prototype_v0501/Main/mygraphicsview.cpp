#include "mygraphicsview.h"
#include <QWheelEvent>
#include <QKeyEvent>

#include <QDebug>
#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

myGraphicsView::myGraphicsView(QWidget *parent):
    QGraphicsView(parent),
    m_translateButton(Qt::LeftButton),
    m_scale(1.0),
    m_zoomDelta(0.1),
    m_translateSpeed(1.0),
    m_bMouseTranslate(false)
{
    // 去掉滚动条
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 设置鼠标光标
    setCursor(Qt::PointingHandCursor);
    // 设置抗锯齿
    setRenderHint(QPainter::Antialiasing);
    // 设置场景的大小
//    setSceneRect(-INT_MIN/2, -INT_MIN/2, INT_MAX, INT_MAX);
    centerOn(0, 0);
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::setTranslateSpeed 设置平移速度
 * @param speed 速度默认1.0 范围0-2
 */
void myGraphicsView::setTranslateSpeed(qreal speed)
{
    // 建议速度范围
    Q_ASSERT_X(speed >= 0.0 && speed <= 2.0,
               "InteractiveView::setTranslateSpeed", "Speed should be in range [0.0, 2.0].");
    m_translateSpeed = speed;
}

qreal myGraphicsView::translateSpeed() const
{
    return m_translateSpeed;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::setZoomDelta 设置缩放增量
 * @param delta 默认0.1 范围0-1
 */
void myGraphicsView::setZoomDelta(qreal delta)
{
    // 建议增量范围
    Q_ASSERT_X(delta >= 0.0 && delta <= 1.0,
               "InteractiveView::setZoomDelta", "Delta should be in range [0.0, 1.0].");
    m_zoomDelta = delta;
}

qreal myGraphicsView::zoomDelta() const
{
    return m_zoomDelta;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::keyPressEvent 上/下/左/右键向各个方向移动、加/减键进行缩放、空格/回车键旋转
 * @param event
 */
void myGraphicsView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Up:
            translate(QPointF(0, -2));  // 上移
            break;
        case Qt::Key_Down:
            translate(QPointF(0, 2));  // 下移
            break;
        case Qt::Key_Left:
            translate(QPointF(-2, 0));  // 左移
            break;
        case Qt::Key_Right:
            translate(QPointF(2, 0));  // 右移
            break;
        case Qt::Key_Plus:  // 放大
            zoomIn();
            break;
        case Qt::Key_Minus:  // 缩小
            zoomOut();
            break;
//        case Qt::Key_Space:  // 逆时针旋转
//            rotate(-5);
//            break;
//        case Qt::Key_Enter:  // 顺时针旋转
//        case Qt::Key_Return:
//            rotate(5);
//            break;
        default:
            QGraphicsView::keyPressEvent(event);
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::mouseMoveEvent 平移
 * @param event
 */
void myGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMouseTranslate){
        QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        translate(mouseDelta);
    }
    m_lastMousePos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}

void myGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == m_translateButton) {
        // 当光标底下没有 item 时，才能移动
        QPointF point = mapToScene(event->pos());
        if (scene()->itemAt(point, transform()) == nullptr)  {
            m_bMouseTranslate = true;
            emit showTranslateBtn(true);
            m_lastMousePos = event->pos();
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void myGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // 左键释放
    if (event->button() == m_translateButton){
        m_bMouseTranslate = false;
        emit showTranslateBtn(false);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::wheelEvent 放缩，保证鼠标在滚动的时候始终在缩放的中心
 * @param event
 */
void myGraphicsView::wheelEvent(QWheelEvent *event)
{
    // 获取当前的鼠标所在的view坐标;
//    QPoint prev_viewPos = event->pos();
    // 获取当前鼠标相对于scene的位置;
//    QPointF prev_scenePos = this->mapToScene(prev_viewPos);

    // 滚轮的滚动量
    QPoint scrollAmount = event->angleDelta();
    // 正值表示 滚轮远离使用者 （放大），负值表示 朝向使用者 （缩小）
    scrollAmount.y() > 0 ? zoomIn() : zoomOut();

    //调整scene，使得scene和view一直，主要是为了排除掉scroll
//    this->scene()->setSceneRect(this->mapToScene(this->rect()).boundingRect());

    //获取缩放后的scene坐标
//    QPointF scenePos = this->mapToScene(prev_viewPos);
    //获取缩放前后的坐标差值，即为需要进行move的位移
//    QPointF disPointF = scenePos - prev_scenePos;
//    qDebug()<<prev_scenePos<<" ::: "<<scenePos<<disPointF;
//    qDebug()<<this->scene()->sceneRect();

    //调整位置scene，使得scene和view一直一样大，排除掉scroll
//    this->scene()->setSceneRect(this->scene()->sceneRect().x()-disPointF.x(),this->scene()->sceneRect().y()-disPointF.y(),
//                                this->scene()->sceneRect().width(),this->scene()->sceneRect().height());
    this->scene()->update();
}

void myGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "拖拽出视图啦";
}

// 每次增加0.1
void myGraphicsView::zoomIn()
{
    zoom(1 + m_zoomDelta);
}

// 每次减少0.1
void myGraphicsView::zoomOut()
{
    zoom(1 - m_zoomDelta);
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-18
 * @brief myGraphicsView::zoom 视图缩放
 * @param scaleFactor scaleFactor：缩放的比例因子
 */
void myGraphicsView::zoom(float scaleFactor)
{
    // 防止过小或过大
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    // 最小的比例是1，也就是原图大小，最大放大5倍
    if (factor < 1 || factor > 5)
        return;

    scale(scaleFactor, scaleFactor);
    m_scale *= scaleFactor;
}

void myGraphicsView::translate(QPointF delta)
{
    // 根据当前 zoom 缩放 平移数
    delta *= m_scale;
    delta *= m_translateSpeed;

    // view 根据鼠标下的点作为锚点来定位 scene
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    QPoint newCenter(VIEW_WIDTH / 2 - delta.x(),  VIEW_HEIGHT / 2 - delta.y());
    centerOn(mapToScene(newCenter));

    // scene 在 view 的中心点作为锚点
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}


