#ifndef PPIWIDGET_H
#define PPIWIDGET_H

#include <QPainter>
#include <QWidget>

#define HORI 180
#define MARGIN 80
#define PIEANG 180  // 余辉持续宽度

namespace Ui
{
class PPIWidget;
}

// 自定义结构体
// 用于保存随机生成的测试点迹
struct Target
{
    qreal distant;//目标与雷达的距离
    int alpha; //透明度
    int angle; //角度
    Target(qreal d,int alp,int ang) : distant(d),alpha(alp),angle(ang) { }
};

/**
 * @brief The PPIWidget class p显窗口显示的主控件
 */
struct Point:Target
{
    QPointF point; // 点的位置(x,y)
    qreal size;    //点迹显示大小
    //int alpha; // 透明度
    //int angle; // 角度
    Point(const Target &t,const QPointF &p,qreal s = 2.0) : Target(t),point(p),size(s) { }
};

enum Mode { Normal,OffCenter,BShow };

class PPIWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PPIWidget(QWidget *parent = nullptr);
    ~PPIWidget();
    void start();
    void stop();
    void get_data4m_points();
    void get_targets();
    void setRange(int range);
    void setImageSize(int value);
    void setShowMode(Mode m);
    void setOffCenterShow();
    void drawPShow();
    void drawBShow();

public slots:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    Ui::PPIWidget *ui;
    bool m_drawable = false;
    int m_angle = 0;
    // 存放点迹的列表
    QList<Target> m_targets;
    QList<Point> *m_points[360]; //存储绘制点迹
    QTimer *m_updateTimer;
    QTimer *timer;
    int radius;
    int w_width, w_height;
    // 雷达量程
    int m_range;
    // 存放计算点迹亮度衰减的系数
    qreal exp_decay[PIEANG];
    // 开启扫描之后计数
    int scan_count;
    int image_size;
    QPointF click_position;
    QPointF mouse_position;
    Mode mode = Normal;
    bool clicked = false;
};
#endif // PPIWIDGET_H
