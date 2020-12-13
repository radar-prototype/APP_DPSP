#include "hellodialog.h"
#include "ui_hellodialog.h"
#include "mainwindownew.h"

#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QStyledItemDelegate>
#include "utils.h"
#include "./page_structure/store.h"
#include "./page_structure/left_side_switch/left_utils.h"
#include "./page_structure/left_side_switch/entrance_widget.h"
/**
* @projectName   prototype_v0719
* @brief         开始欢迎页面，也即是进入主程序之前的拦截界面。可以选择打开或者新建工程。
* @author        Antrn
* @date          2019-08-12
*/
HelloDialog::HelloDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelloDialog)
{
    ui->setupUi(this);
    // 在main.cpp中的栈上新建此对话框的时候 写下一行这个在projectlist发出信号关闭的时候会出现错误，因为栈中的变量，编译器会自动释放
    // this->setAttribute(Qt::WA_DeleteOnClose);
    //应用样式 apply the qss style
    QFile file(":/qss/qss_hellodialog.qss");
    file.open(QIODevice::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
//    qDebug() << stylesheet;
    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
    ui->comboBox->setItemDelegate(itemDelegate);
    ui->comboBox->setStyleSheet(stylesheet);
    file.close();

    ui->comboBox->setView(new QListView(this));
    //去掉页面边框和标题栏
    // 5.6.1
    this->setWindowFlags(Qt::FramelessWindowHint);
    // 5.9.8
    // this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowIcon(QIcon(":/img/HI.png"));
    //标题栏保留，去掉大小按钮
//    this->setWindowFlag(Qt::WindowMinimizeButtonHint, false);
//    this->setWindowFlag(Qt::WindowMaximizeButtonHint, false);
//    去除所有按钮
//    this->setWindowFlags(Qt::WindowTitleHint|Qt::CustomizeWindowHint);
    //只剩关闭按钮
//    this->setWindowFlag(Qt::WindowCloseButtonHint);

    QString styleSheet =
        //正常状态样式
        "QPushButton{"
        "background-color:rgba(211, 215, 207, 30);"//背景色（也可以设置图片）
        "border-style:outset;"                  //边框样式（inset/outset）
        "border-width:1px;"                     //边框宽度像素
        "border-radius:5px;"                   //边框圆角半径像素
        "border-color:rgba(0,0,0,30);"    //边框颜色
        "}"
        //鼠标按下样式
        "QPushButton:pressed{"
        "background-color:rgba(100,255,100,200);"
        "border-color:rgba(255,255,255,200);"
        "border-style:inset;"
        "}"

        //鼠标悬停样式
        "QPushButton:hover{"
        "background-color:rgba(255,255,255,80);"
        "border-color:rgba(30,30,30,100);"
        "}";
    ui->pushButton_open->setStyleSheet(styleSheet);
    ui->pushButton_new->setStyleSheet(styleSheet);
    // 关闭按钮
//    ui->pushButton->setStyleSheet(styleSheet);
    ui->pushButton->setVisible(false);//不需要了隐藏

    // Antrn: 设置边框 然而没有什么用 who can help me?
//    setStyleSheet("border:1px solid black;");
//    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
//    shadow_effect->setOffset(0, 0);
//    shadow_effect->setColor(Qt::black);
//    shadow_effect->setBlurRadius(10);
//    setGraphicsEffect(shadow_effect);
//    this->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->projectList, &ProjectList::set_window, this, &HelloDialog::set_window);

    if(createProListFile())
        qDebug() << "初始化创建.pl文件成功";
    else
        qDebug() << "初始化创建.pl文件失败";
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-20
 * @brief HelloDialog::createProListFile 程序启动自动新建文件列表文件，是空的
 */
bool HelloDialog::createProListFile()
{
    // 创建project文件夹
    QString proDir = QDir::currentPath() + "/project/";
    Utils::openDirOrCreate(proDir);

    QString fpath = proDir + "projectlist.pl";
    QFile file(fpath); //相对路径、绝对路径、资源路径都可以
    if(!file.exists())
    {
        if(!file.open(QFile::WriteOnly))
        {
            qDebug() << "无法新建pl文件";
            return false;
        }
        file.close();
        QDomDocument doc;
        QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.appendChild(instruction);
        QDomElement root = doc.createElement("projects");
        doc.appendChild(root);

        // 【千万要注意】，改动doc之后要手动保存，我在这上面绊了一脚
        QFile file2(fpath);
        // 每次打开文件都会先删除后重写
        if(!file2.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug() << "打开准备初始化.pl文件失败";
            return false;
        }
        else
        {
            QTextStream out(&file2);
            doc.save(out, 4); //EncodingFromDocument
            file2.close();
            return true;
        }
    }
    else
    {
        qDebug() << ".pl已经存在!!";
        return true;
    }
}

/**
 * @brief 打开工程
 */
void HelloDialog::on_pushButton_open_clicked()
{
//    QProcess process;
//    process.startDetached(QStringLiteral("explorer.exe /select,\"E:\\Desktop\\mao.txt\""));

    // 用户选择dpsp的路径
    QString ppath = QFileDialog::getOpenFileName(nullptr, tr("打开工程.dpsp"), QDir::currentPath() + "/project/", "Project Files(*.dpsp)");
    if(!ppath.isEmpty())
    {
        //shao:新建或返回整体仿真窗口交给store类处理
//        MainWindow* window=Store::open_main_window(ppath);
         QMainWindow* window1=Store::open_window(Page_structure_utils::main,ppath);
         if(window1==nullptr) return;
         MainWindow* window = dynamic_cast<MainWindow*>(window1);
         connect(window, &MainWindow::set_window, this, &HelloDialog::set_window);
         emit set_window(Left_utils::whole_simulation,  Left_utils::switch_update, window);  //左边切换
    }
}

/**
 * 新建工程
 */
void HelloDialog::on_pushButton_new_clicked()
{
    //shao:新建或返回整体仿真窗口交给store类处理
//    MainWindow* mainnew = Store::add_main_window();
     QMainWindow* mainnew = Store::add_window(Page_structure_utils::main);
     MainWindow *pB = dynamic_cast<MainWindow*>(mainnew);//转换类型
    connect(pB, &MainWindow::set_window, this, &HelloDialog::set_window);
    emit set_window(Left_utils::whole_simulation, Left_utils::switch_update, mainnew);
}


