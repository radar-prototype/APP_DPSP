#include <QApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QDateTime>
#include <QLabel>
#include <QMovie>
#include "unistd.h" //usleep()
#include "hellodialog.h"
#include "mysplashscreen.h"
#include <QStyleFactory>
#include "./RadarPrototype/component/component_management_window.h"
#include "./Tool/page_structure/real_home_page.h"
/**
* @projectName   prototype_v0719
* @brief         此程序的main函数，入口类。
* @author        Antrn
* @date          2019-08-12
*/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//    快捷开发暂时注释
{
//   app.setStyle(QStyleFactory::create("vis")); // 设置vista风格
////    QPixmap pixmap(":/img/start_logo.gif");
//   QPixmap pixmap(":/img/startup.gif");
////    QPixmap pixmap(":/img/lixian.jpg");
//   MySplashScreen splash(pixmap, 1500);
////    QSplashScreen splash(pixmap);
////    splash.setWindowOpacity(0.5);

//   QLabel label(splash.w);
//   QMovie mv(":/img/startup.gif");
////    QMovie mv(":/img/start_logo.gif");
//   label.setMovie(&mv);
//   mv.start();
//   splash.show();
//   splash.setCursor(Qt::BlankCursor);

//   for (int i=0; i<100; i++) {
//       app.processEvents();
//       usleep(15000);
//   }
////    QDateTime n=QDateTime::currentDateTime();
////    QDateTime now;
////    do{
////        now=QDateTime::currentDateTime();
////    } while (n.secsTo(now)<=5);//6为需要延时的秒数

////    for(int i=0; i<5000; i+=mv.speed())
////    {
////        QCoreApplication::processEvents();
////        usleep(500*static_cast<__useconds_t>(mv.speed()));
////    }
//   HelloDialog* hello=new HelloDialog();
//   hello->show();
//   splash.finish(hello);

//   //    MainWindow window;
//   //    window.show();
//   //    splash.finish(&window);
}
//用来看其他界面
 {
    //  快捷打开看雷达页面
    // QString id="雷达43";
    // MainWindow_Radar *SET_RADARNAME=new MainWindow_Radar(id);

    // SET_RADARNAME->setAttribute(Qt::WA_DeleteOnClose);
    // SET_RADARNAME->setWindowIcon(QIcon(":/img/radar.png"));
    // SET_RADARNAME->show();

    //组件管理窗口
//    component_management_window *windows=new component_management_window();
//    windows->show();

//   新的主页加载
     Real_home_page* real_home_page =new Real_home_page();
     real_home_page->showWidget();
}
    return app.exec();
}
