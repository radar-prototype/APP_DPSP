#include "mainwindow_ecm.h"
#include "ui_mainwindow_ecm.h"

#include<QLabel>
#include <QDebug>
#include<QGraphicsScene>
#include "graphicsscenepublic.h"
#include <QToolButton>
#include "compproperty.h"
MainWindow_ECM::MainWindow_ECM(QString id , QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_ECM)
{
    ui->setupUi(this);

    //控制台打印id："对抗2"
    qDebug() <<"打开了"+ id ;
    //TODO 用法不清楚
    this->setAttribute(Qt::WA_DeleteOnClose);
    //设置刚打开全屏显示
//   this->showMaximized();
    //使用公共场景
//   GraphicsScenePublic *sceneECM = new GraphicsScenePublic(this->itemMenu);
    sceneECM= new GraphicsScenePublic(this->itemMenu);
    //设置图形窗体大小（不设拖控件会出问题）
    sceneECM->setSceneRect(QRectF(0,0,5000,5000));
    // 用来显示xy坐标的标签
    label_xy=new QLabel(this);
    // 状态栏左下角加入xy坐标
//    默认定位到算法组件
    ui->toolBox->setCurrentIndex(0);
    ui->statusbar->addWidget(label_xy);
    // 设置鼠标跟踪开启
    ui->graphicsView_ecm->setMouseTracking(true);

    ui->graphicsView_ecm->setAcceptDrops(true);
    // 鼠标移动触发xy更新
    connect(sceneECM,&GraphicsScenePublic::signal_xy,this,[=](double x,double y){
        this->xy_show(x,y);
    });
    connect(sceneECM, SIGNAL(itemInserted(DiagramItemPublic*)),
            this, SLOT(itemInserted(DiagramItemPublic*)));
//    connect(sceneECM, SIGNAL(textInserted(QGraphicsTextItem*)),
//            this, SLOT(textInserted(QGraphicsTextItem*)));
    connect(sceneECM, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(itemSelected(QGraphicsItem*)));
    connect(sceneECM,SIGNAL(startRun()),
            this,SLOT(On_start()));
    connect(sceneECM,SIGNAL(overRun()),
            this,SLOT(On_over()));
    ui->graphicsView_ecm->setScene(sceneECM);

    //设置可以使鼠标在视图上拖拽出橡皮筋框选择多个组件
    ui->graphicsView_ecm->setDragMode(QGraphicsView::RubberBandDrag);
    //点击run菜单项（或工具栏图标）触发
    connect(ui->action_run,SIGNAL(triggered()),
            sceneECM,SLOT(startRunCode()));
    connect(ui->action_run_2,SIGNAL(triggered()),
            sceneECM,SLOT(startRunCode()));
    //点击退出菜单项触发
    connect(ui->actiont_exit, SIGNAL(triggered()), this, SLOT(close()));
    //加入算法组件
    createCompBox();
    createToolbars();
}
MainWindow_ECM::~MainWindow_ECM()
{
    delete ui;
}

//设置左下角坐标
void MainWindow_ECM::xy_show(double x, double y)
{
    QString str=QString("x=%1,y=%2").arg(QString::number(x)).arg(QString::number(y));
    label_xy->setText(str);
}

void MainWindow_ECM::createCompBox(){
    buttonGroup = new QButtonGroup(ui->dockWidgetContents);
    buttonGroup->setExclusive(true);
    //表格布局
    QGridLayout *layout = new QGridLayout;
    // TODO 支持点击修改组件名
    layout->addWidget(createCellWidget(tr("*脉冲压缩"), DiagramItemPublic::Comp1), 0, 0); // Component_1
    layout->addWidget(createCellWidget(tr("*恒虚警率"), DiagramItemPublic::Comp2),0, 1); // Component_2
    layout->addWidget(createCellWidget(tr("*动目标检测"), DiagramItemPublic::Comp4), 1, 0); // Component_4
    layout->addWidget(createCellWidget(tr("输入"), DiagramItemPublic::Comp3), 1, 1); // Component_3
    layout->addWidget(createCellWidget(tr("输出"), DiagramItemPublic::Comp5), 2, 0); // Component_5

    //设置行和列的比例
    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    ui->listWidget_arithmetic->setLayout(layout);
    //自定义的slot函数：与graphics交互的关键（id--点的哪个传哪个 QString类型）
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
};

QWidget *MainWindow_ECM::createCellWidget(const QString &text, DiagramItemPublic::DiagramType type)
    {
        DiagramItemPublic item(type, itemMenu);
        QIcon icon(item.image());
        //专门用来显示图片的按钮组件
        QToolButton *button = new QToolButton;
        button->setIcon(icon);
        button->setIconSize(QSize(50, 50));
        button->setCheckable(true);
        buttonGroup->addButton(button, int(type));
       //名称和按钮在一个表格布局里
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(button, 0, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);
        QWidget *widget = new QWidget();
        widget->setLayout(layout);
    //返回一个含有名称的toolbutton---->是否该换成自定义控件？
        return widget;
    }

void MainWindow_ECM::createToolbars()
{
 //箭头和连线
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    //默认是指针移动
    pointerButton->setIcon(QIcon(":/img/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/img/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    //后面参数是id唯一标识（String强转int？--->好方法 好记不会错）
    pointerTypeGroup->addButton(pointerButton, int(GraphicsScenePublic::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(GraphicsScenePublic::InsertLine));
    //一旦有按钮选中-->鼠标就变箭头（不再是连线）
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));
     ui->toolBar->addWidget(pointerButton);
     ui->toolBar->addWidget(linePointerButton);

}
//参数呢？
void MainWindow_ECM::pointerGroupClicked(int id)
{
    qDebug()<<"pointerGroupClickedid:  "<<id;
    sceneECM->setMode(GraphicsScenePublic::Mode(pointerTypeGroup->checkedId()));
}
void MainWindow_ECM::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup->button(id) != button)
            button->setChecked(false);
    }
    //判断是否是文本框
    if (id == 10) { //InsertTextButton=10 我没定义
        //sceneECM->setMode(GraphicsScenePublic::InsertText);
    } else {
        sceneECM->setItemType(DiagramItemPublic::DiagramType(id));
        sceneECM->setMode(GraphicsScenePublic::InsertItem);
    }
}
//按理说应该修改样式让被选中的算法组件明显一点，还没做
void MainWindow_ECM::itemSelected(QGraphicsItem *item)
{
    qDebug()<<"itemSelected";
//    DiagramTextItem *textItem =
//    qgraphicsitem_cast<DiagramTextItem *>(item);

//    QFont font = textItem->font();
//    fontCombo->setCurrentFont(font);
//    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
//    boldAction->setChecked(font.weight() == QFont::Bold);
//    italicAction->setChecked(font.italic());
//    underlineAction->setChecked(font.underline());
}

void MainWindow_ECM::itemInserted(DiagramItemPublic *item)
{

//    isSave = false;
    qDebug() << "ECM item Inserted finish";
    //当有图形项插入到场景中的时候，应该将指针组改为移动指针
   pointerTypeGroup->button(int(GraphicsScenePublic::MoveItem))->setChecked(true);
    //设置成上面那个
   sceneECM->setMode(GraphicsScenePublic::Mode(pointerTypeGroup->checkedId()));
   //sceneECM->setMode(GraphicsScenePublic::MoveItem);
    //取消原按钮的选中状态
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
    update();
//    ui->toolBox->update();
}

//没有使用
void MainWindow_ECM::sceneScaleChanged(const QString &scale)
{
//    isSave = false;
    qDebug() << "1117";
   // double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
//    QMatrix oldMatrix = ui->graphicsView->matrix();
//    ui->graphicsView->resetMatrix();
//    ui->graphicsView->translate(oldMatrix.dx(), oldMatrix.dy());
//    ui->graphicsView->scale(newScale, newScale);
}


//打开关闭左边算法组件窗口
void MainWindow_ECM::on_action_list_triggered(bool checked)
{
    if(checked){
        ui->dockWidget->show();
    }else{
        ui->dockWidget->hide();
    }
}
//打开关闭toolBar
void MainWindow_ECM::on_action_toolBar_triggered(bool checked)
{
    if(checked){
        ui->toolBar->show();
    }else{
        ui->toolBar->hide();
    }
}

void MainWindow_ECM::On_start()
{
    // TODO 删除或者完善
}

void MainWindow_ECM::On_over()
{
    // TODO 删除或者完善
}

//弹出属性窗口
void MainWindow_ECM::showItemProperties()
{
    foreach (QGraphicsItem *item, sceneECM->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            // DELETE
            qDebug() << "Show Component Property";
            CompProperty *p = new CompProperty("hahh");
            p->exec();
        }
    }
}
//进度条相关
// 当已经开始运行的时候
//void MainWindow_ECM::On_start()
//{
//    // 关闭几个按钮和动作
//    ui->actionRunRadar->setEnabled(false);
//    // 打开停止按钮和动作
////    ui->action_Stop->setEnabled(true);
//    // 进度条为0
//    progressBar->setValue(0);
//}

// 当运行结束的时候
//void MainWindow_ECM::On_over()
//{
////    ui->action_Stop->setEnabled(false);
//    ui->actionRunRadar->setEnabled(true);
//    label_time->setText(tr("Done"));
//}

// 当对信号进行评估进度的时候，实时设置进度条的值
//void MainWindow_ECM::On_rateSignal(float rate)
//{
//    progressBar->setValue(int(rate));
//}

