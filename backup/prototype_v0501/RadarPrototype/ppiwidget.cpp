#include "ppiwidget.h"
#include "ui_ppiwidget.h"
#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include <QMouseEvent>
#include <QLabel>

const static double PI = 3.1415926;

PPIWidget::PPIWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PPIWidget)
{
    ui->setupUi(this);
    //setStyleSheet("background-color:rgb(0,0,0)");
    // 5.9.8
    qsrand(uint (time_t(nullptr)));
    // 5.6.1
    // qsrand(uint (time(nullptr)));
    // 定时器
    m_updateTimer = new QTimer(this);
    // 更新定时器 间隔16ms
    m_updateTimer->setInterval(16);
    // 超时信号槽，更新
    connect(m_updateTimer, &QTimer::timeout, this, [this]()
    {
        // 重绘窗口 update()比repaint()更好，因为它允许Qt来优化速度并且防止闪烁
        // repaint()是立即调用paintEvent()，而update()是几次执行才调用一次paintEvent()
        update();
    });

    // 绘画定时器
    timer = new QTimer(this);
    w_width = width();
    w_height = height();

    // 计算宽和高的较小者，减去margin MARGIN的二分之一作为半径
    radius = (qMin(w_width, w_height) - MARGIN) / 2;
/*
    connect(timer, &QTimer::timeout, this, [ = ]
    {
        // 点迹数组不为空
        if(!m_points.isEmpty())
        {
            // 每次删除一个，添加一个
            for(int i = 0; i < 200; ++i)
            {
                m_points.removeAt(i);
                int alpha = qrand() % 100 + 40;
//                int ang = -30;
                // 径向距离
                int radiu = qrand() % radius;
                // 距离正北方向顺时针的角度
                int angle = qrand() % 360;
                double px = radiu * cos(angle * qvariant_cast<double>(PI / HORI));
                double py = radiu * sin(angle * qvariant_cast<double>(PI / HORI));
                // PPI中心 雷达的位置
                QPoint center(int(w_width / 2), int(w_height / 2));
                m_points.append(Point(QPointF(center.x() + px, center.y() + py), alpha, angle));
            }
        }
        else
        {
            get_data4m_points();
        }
    });
*/

    //量程初始为1Km
    m_range = 1000;
    image_size = 50;

    for(int i = 0;i < 360;++i)
    {
        m_points[i] = nullptr;
    }
    // 初始化亮度衰减系数
    for(int i=0;i<PIEANG;++i)
    {
        qreal k = qLn(0.05) / PIEANG; //余辉持续衰减到初始亮度的5%
        exp_decay[i] = qExp(k * i);
    }
    setMouseTracking(true);
}

PPIWidget::~PPIWidget()
{
    for(int i=0;i<360;++i)
    {
        if(m_points[i] != nullptr) delete m_points[i];
    }
    delete ui;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-08
 * @brief PPIWidget::start 点击开始仿真
 */
void PPIWidget::start()
{
    get_targets();
    m_drawable = true;
    m_angle = 90;
    scan_count = 0;
    // 绘画间隔 1s
    timer->start(1000);
    // 16ms
    m_updateTimer->start();
}

void PPIWidget::stop()
{
    m_drawable = false;
    timer->stop();
    m_updateTimer->stop();
    m_targets.clear();
    for(int i=0;i<360;++i)
    {
        if(m_points[i] != nullptr)
        {
            delete m_points[i];
            m_points[i] = nullptr;
        }
    }
    //m_points.clear();
    update();
}

/**
* @author  fan
* @date    2020-07-24
* @brief   在最大量程范围内生成1000条目标数据
*/
void PPIWidget::get_targets()
{
    for(int i = 0;i < 1000;++i)
    {
        qreal distant = qrand() % 6000;
        int alpha = qrand() % 100 + 155;
        int angle = qrand() % 360;
        m_targets.append(Target(distant,alpha,angle));
    }
}

/**
* @author  fan
* @date    2020-07-24
* @brief   改变雷达量程
*/
void PPIWidget::setRange(int range)
{
    m_range=range;
    if(m_targets.empty())
    {
        get_targets();
    }
    //m_points.clear();
    //get_data4m_points();
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-08
 * @brief PPIWidget::get_data4m_points 为了m_points生成数据
 */
void PPIWidget::get_data4m_points()
{
    if(m_points[m_angle] == nullptr) m_points[m_angle] = new QList<Point>;
    m_points[m_angle]->clear();
    // 最一开始生成1000条数据
    // 根据当前量程和扫描线方向对数据处理
    for(int i = 0; i < 1000; ++i)
    {
        // 将点迹方位角转换为(3点钟方向为0度，逆时针方向为正)
        int angle = (450 - m_targets.at(i).angle) % 360;

        if(m_angle != angle) continue;
        if(m_targets.at(i).distant > m_range) continue;

        double radiu = m_targets.at(i).distant/m_range*radius;
        double px = radiu * cos(- angle * qvariant_cast<double>(PI / HORI));
        double py = radiu * sin(- angle * qvariant_cast<double>(PI / HORI));
        QPoint center(int(w_width / 2), int(w_height / 2));
        m_points[angle]->append(Point(m_targets[i],QPointF(center.x() + px, center.y() + py),2 * qSqrt(qvariant_cast<qreal>(image_size) / 100)));
    }
}

/**
* @author  fan
* @date    2020-07-24
* @brief   设定图像显示比例
*/
void PPIWidget::setImageSize(int value)
{
    image_size = value + 1;
}

/**
* @author  fan
* @date    2020-07-31
* @brief   设置图像显示模式
*/
void PPIWidget::setShowMode(Mode m)
{
    mode = m;
    clicked = false;
}

void PPIWidget::drawPShow()
{
    QPainter painter;
    QPixmap pixmap(this->size());
    painter.begin(&pixmap);
    // 格子
//    for(int i = 0; i < 500; i += 20)
//        painter.drawLine(i + 0.5, 0, i + 0.5, 500);
//    for(int j = 0; j < 500; j += 20)
//        painter.drawLine(0, j + 0.5, 500, j + 0.5);

    int min = int(qMin(width(), height()) - MARGIN);
    QPoint center(int(width() / 2), int(height() / 2));
    pixmap.fill(this->palette().background().color());

    // 绘制黑色背景
    painter.setBrush(Qt::black);
    painter.drawEllipse(center, min / 2, min / 2);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(230, 230, 250, 250), 0.5));
    // 画三个环
    painter.drawEllipse(center, min / 2, min / 2);
    painter.drawEllipse(center, min / 3, min / 3);
    painter.drawEllipse(center, min / 6, min / 6);
    // 横竖直线
    painter.drawLine(QPointF(center.x(), center.y() - min / 2), QPointF(center.x(), center.y() + min / 2));
    painter.drawLine(QPointF(center.x() - min / 2, center.y()), QPointF(center.x() + min / 2, center.y()));
    // 撇捺直线
    painter.drawLine(QPointF(center.x() - ((min / 2) / sqrt(2)), center.y() - (min / 2) / sqrt(2)), QPointF(center.x() + (min / 2) / sqrt(2), center.y() + (min / 2) / sqrt(2)));
    painter.drawLine(QPointF(center.x() + ((min / 2) / sqrt(2)), center.y() - (min / 2) / sqrt(2)), QPointF(center.x() - (min / 2) / sqrt(2), center.y() + (min / 2) / sqrt(2)));

    // 大刻度
    for (int i = 0; i < 360; i += 10)
    {
        qreal x = center.x() + min / 2 * cos((i - 90) * 3.14159265 / HORI);
        qreal y = center.y() + min / 2 * sin((i - 90) * 3.14159265 / HORI);
        QPointF p(x - 10 * cos((i - 90) * 3.14159265 / HORI), y - 10 * sin((i - 90) * 3.14159265 / HORI));
        painter.drawLine(p, QPointF(x, y));
    }
    // 小刻度
    for (int i = 0; i < 360; i += 1)
    {
        qreal x = center.x() + min / 2 * cos((i - 90) * 3.14159265 / HORI);
        qreal y = center.y() + min / 2 * sin((i - 90) * 3.14159265 / HORI);
        QPointF p(x - 5 * cos((i - 90) * 3.14159265 / HORI), y - 5 * sin((i - 90) * 3.14159265 / HORI));
        painter.drawLine(p, QPointF(x, y));
    }

//    for (int i=0; i<HORI;i+=10) {
//        qreal x = center.x() + 250 * cos((i-90)*3.14159265/HORI);
//        qreal y = center.y() + 250 * sin((i-90)*3.14159265/HORI);
//        QPointF p2(x+10 * cos((i-90)*3.14159265/HORI), y+20 * sin((i-90)*3.14159265/HORI));
//        if(i<100 && i!=0)
//            painter.drawText(p2, '0'+QString::number(i));
//        else
//            painter.drawText(p2, QString::number(i));
//    }

    QFont font;
    font.setPointSize(10);
    // 刻度上数字
    for (int i = 0; i < 360; i += 10)
    {
        qreal x = center.x() + min / 2 * cos((i - 90) * 3.14159265 / HORI);
        qreal y = center.y() + min / 2 * sin((i - 90) * 3.14159265 / HORI);

        painter.setFont(font);
        painter.setPen(Qt::black);
        QFontMetrics fm = painter.fontMetrics();
        int width = fm.width(QString::number(i));
        int height = fm.ascent() + fm.descent();
        QRectF rect(x + 18 * cos((i - 90) * 3.14159265 / HORI) - width / 2, y + 18 * sin((i - 90) * 3.14159265 / HORI) - height / 2, width, height);
        painter.drawText(rect, QString::number(i));
    }

    if (m_drawable)
    {
        int diff = int(qAbs(width() - height()) / 2) + (MARGIN / 2);
        int scan_angle = qMin(scan_count,PIEANG);
        // 画一条扫描线
        painter.setPen(QPen(QColor(34, 214, 78, 180),2));
        double px = radius * cos(- m_angle * qvariant_cast<double>(PI / HORI));
        double py = radius * sin(- m_angle * qvariant_cast<double>(PI / HORI));
        painter.drawLine(center.x(),center.y(),center.x() + qvariant_cast<int>(px),center.y() + qvariant_cast<int>(py));
        // 扫描线余辉，圆锥渐变
        QConicalGradient gradient(center.x(), center.y(), m_angle);
        for(int i = 0;i < scan_angle;++i)
        {
            if(i % 5 != 0) continue;
            gradient.setColorAt(qvariant_cast<qreal>(i) / 360, QColor(34, 214, 78, qvariant_cast<int>(180 * exp_decay[i])));
        }
        //gradient.setColorAt(0.1, QColor(34, 214, 78, 255));
        //gradient.setColorAt(0.3, QColor(34, 214, 78, 140));
        //gradient.setColorAt(0.5, QColor(34, 214, 78, 0));
        painter.setBrush(gradient);
        painter.setPen(QPen(Qt::transparent));
        if (width() > height())
            // 角度单位1/16,扇形是60度
            painter.drawPie(diff, MARGIN / 2, min, min, m_angle * 16, scan_angle * 16);
        else
            painter.drawPie(MARGIN / 2, diff, min, min, m_angle * 16, scan_angle * 16);

        // 点迹及余辉
        get_data4m_points();
        for(int i = 0;i < scan_angle + 1;++i)
        {
            int angle = (m_angle + i) % 360;
            //if(m_points[angle] == nullptr) break;

            //绘画点迹及余辉
            if(m_points[angle]->isEmpty()) continue;
            for(Point p:*m_points[angle])
            {
                int alpha = qvariant_cast<int>(p.alpha * exp_decay[i]);
                painter.setBrush(QBrush(QColor(34, 214, 78, alpha)));
                painter.drawEllipse(p.point,p.size,p.size);
            }
        }
        /* 之前显示点迹的方法，暂时注释掉
        if(!m_points.isEmpty())
        {
            for(int i = 0; i < m_points.size(); ++i)
            {
                painter.setBrush(QBrush(QColor(34, 214, 78, m_points.at(i).alpha)));
                // 画点
//                painter.drawEllipse(m_points.at(i).point, 3, 3);
                if((m_points.at(i).angle + 60) % 360 <= (abs(m_angle - PIEANG) % 360) && (m_points.at(i).angle + 60) % 360 >= (abs(m_angle) % 360))
                    //painter.drawChord(m_points.at(i).point.x() - 28, m_points.at(i).point.y() - 10, 30, 10, m_points.at(i).angle * 16, 60 * 16);
                    painter.drawEllipse(m_points.at(i).point,3,1);
            }
        }
        */
        m_angle = (m_angle - 1 + 360) % 360;
        //qDebug() << m_angle;
        scan_count += 1;
    }
    painter.end();

    painter.begin(this);

    if(mode == OffCenter && clicked)
    {
        qreal x = qPow(abs(click_position.x() - center.x()),2);
        qreal y = qPow(abs(click_position.y() - center.y()),2);
        qreal r = radius - qSqrt(x + y);
        QPainterPath path;
        path.addEllipse(center.x() - radius,center.y() - radius,min,min);
        path.addRect(0,w_height / 2 + radius + 10,w_width,(w_height - 2 * radius) / 2 - 10);
        painter.setClipPath(path);

        painter.drawPixmap(center.x() - radius,center.y() - radius,min,min,pixmap,click_position.x() - r,click_position.y() - r,r * 2,r * 2);
    }
    else {
        painter.drawPixmap(0,0,width(),height(),pixmap);
    }

    //显示距离和方位角
    qreal distant = qSqrt(qPow(mouse_position.x() - center.x(),2)+qPow(mouse_position.y() - center.y(),2)) * m_range / radius;
    int angle = qvariant_cast<int>(qAtan((mouse_position.y() - center.y()) / (mouse_position.x() - center.x())) * 180 / PI);
    if(mouse_position.x() > center.x())
    {
        angle += 90;
    }
    else if(qvariant_cast<int>(mouse_position.x()) == center.x())
    {
        if(mouse_position.y() > center.y())
            angle = 180;
        else
            angle = 0;
    }
    else {
        angle = (angle + 270) % 360;
    }
    QString text1 = "方位角：" + QString::number(angle);
    QString text2 = "距离：" + QString::number(distant,'f',2) + "m";
    painter.setFont(font);
    painter.setPen(Qt::black);
    QFontMetrics fm = painter.fontMetrics();
    //int width = fm.width(text);
    int height = fm.ascent() + fm.descent();
    painter.drawText(0,w_height - height,text1);
    painter.drawText(0,w_height - 2 * height,text2);
    painter.end();

}
void PPIWidget::drawBShow()
{
    QPixmap pixmap(this->size());
    QPainter painter;
    painter.begin(&pixmap);
    pixmap.fill(Qt::black);
    //temp.fill(this->palette().background().color());

    int width = w_width - MARGIN;
    int height = w_height - MARGIN;
    //int min = int(qMin(width(), height()) - MARGIN);
    QPointF center(w_width / 2, w_height / 2);
    QPointF buttomLeft(MARGIN / 2,MARGIN / 2 + height);
    //int angle = qvariant_cast<int>(qAtan((click_position.y() - center.y()) / (click_position.x() - center.x())));
    //绘制方格
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen oldPen = painter.pen();
    QPen pen(oldPen);
    pen.setStyle(Qt::DotLine);
    pen.setColor(Qt::white);
    painter.setPen(pen);
    for(int i = 0;i <= 12;++i)
    {
        //qreal line = qSqrt(2) * radius;
        QPointF p1(buttomLeft.x() + width * i / 12,buttomLeft.y());
        QPointF p2(p1.x(),p1.y() - height);
        painter.drawLine(p1,p2);
    }
    for(int i = 0;i < 5;++i)
    {
        //qreal line = qSqrt(2) * radius;
        QPointF p1(buttomLeft.x(),buttomLeft.y() - height * i / 4);
        QPointF p2(p1.x() + width,p1.y());
        painter.drawLine(p1,p2);
    }
    //绘制数字
    QFont font;
    font.setPointSize(10);
    for(int i = 0;i <= 12;++i)
    {
        painter.setFont(font);
        painter.setPen(Qt::white);
        QFontMetrics fm = painter.fontMetrics();
        int w = fm.width(QString::number(i * 30));
        int h = fm.ascent() + fm.descent();
        QRectF  rect(buttomLeft.x() + width * i / 12 - w / 2,buttomLeft.y(),w,h);
        painter.drawText(rect,QString::number(i * 30));
    }
    for(int i = 1; i < 5;++i)
    {
        QString s = QString::number(m_range * i / 4) + 'm';
        painter.setFont(font);
        painter.setPen(Qt::white);
        QFontMetrics fm = painter.fontMetrics();
        int w = fm.width(s);
        int h = fm.ascent() + fm.descent();
        QRectF  rect(buttomLeft.x() - w,buttomLeft.y() - height * i / 4 - h / 2,w,h);
        painter.drawText(rect,s);
    }

    if (m_drawable)
    {
        int scan_angle = qMin(scan_count,PIEANG);
        get_data4m_points();
        for(int i = 0;i < scan_angle + 1;++i)
        {
            int a = (m_angle + i) % 360;
            //if(m_points[angle] == nullptr) break;

            //绘画点迹及余辉
            if(m_points[a]->isEmpty()) continue;
            for(Point p:*m_points[a])
            {
                int angle = p.angle;
                int alpha = qvariant_cast<int>(p.alpha * exp_decay[i]);
                painter.setBrush(QBrush(QColor(34, 214, 78, alpha)));
                painter.setPen(Qt::NoPen);
                QRectF rect(buttomLeft.x() + angle / 360.0 * width,buttomLeft.y() - (p.distant / m_range * height),p.size * 2,p.size * 2);
                painter.drawEllipse(rect);
            }
        }

        m_angle = (m_angle - 1 + 360) % 360;
        scan_count += 1;
    }
    click_position.setX(center.x() + 10);
    click_position.setY(center.y() + 10);
    painter.end();

    painter.begin(this);
    painter.drawPixmap(0,0,w_width,w_height,pixmap);
    painter.end();
}
// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-08
 * @brief PPIWidget::paintEvent 窗口的绘画事件，每次update都会调用
 */
void PPIWidget::paintEvent(QPaintEvent *)
{
    if(mode == BShow)
        drawBShow();
    else {
        drawPShow();
    }
}

void PPIWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    if(mode == OffCenter && !clicked)
    {
        qreal x = qPow(abs(event->pos().x() - w_width / 2),2);
        qreal y = qPow(abs(event->pos().y() - w_height / 2),2);
        if(qSqrt(x + y) > radius)
            return;
        click_position = event->pos();
        clicked = true;
    }
}

void PPIWidget::mouseMoveEvent(QMouseEvent *event)
{
    qreal x = qPow(abs(event->pos().x() - w_width / 2),2);
    qreal y = qPow(abs(event->pos().y() - w_height / 2),2);
    if(qSqrt(x + y) > radius)
        return;
    mouse_position = event->pos();
    if(mode == OffCenter && clicked)
    {
        qreal x = qPow(abs(click_position.x() - w_width / 2),2);
        qreal y = qPow(abs(click_position.y() - w_height / 2),2);
        qreal radio = (radius - qSqrt(x + y)) / radius;
        mouse_position.setX((mouse_position.x() - w_width / 2) * radio + click_position.x());
        mouse_position.setY((mouse_position.y() - w_height / 2) * radio + click_position.y());
    }
}

void PPIWidget::resizeEvent(QResizeEvent *event)
{
    click_position.setX(click_position.x() / w_width * width());
    click_position.setY(click_position.y() / w_height * height());
    w_width = width();
    w_height = height();
    radius = int(qMin(w_width, w_height) - MARGIN) / 2;

    //delete pixmap;
    //pixmap = new QPixmap(this->size());
    QWidget::resizeEvent(event);
}
