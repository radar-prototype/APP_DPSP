#include "store.h"
#include <QMessageBox>
#include <QInputDialog>
#include "utils.h"
#include "page_structure_utils.h"
#include "mainwindow.h"
#include "mainwindow_radar.h"
#include "mainwindow_ecm.h"
#include "object_prototype/mainwindow_object.h"
//初始化
QMap<Page_structure_utils::window_type ,QList<QString>> Store::existing_prototype_name;
QMap<Page_structure_utils::window_type,QMap<QString ,QMainWindow*>>  Store::window_list;

Store::Store(QObject *parent) : QObject(parent)
{
     Store::update_prototype_map();
}

QMap<Page_structure_utils::window_type, QMap<QString, QMainWindow *> > Store::getWindow_list()
{
    return window_list;
}

QMap<Page_structure_utils::window_type, QList<QString> > Store::getExisting_prototype_name()
{
    Store::update_prototype_map();
    return existing_prototype_name;
}

//添加
QMainWindow* Store::add_window(Page_structure_utils::window_type window_type){
    bool isOK;
    QString text = QInputDialog::getText(nullptr, "新建",
                                                       "请输入新建工程/设备名",
                                                       QLineEdit::Normal,
                                                       "请不要和已有工程/设别名重复",
                                                       &isOK);
    if(isOK&&!text.isEmpty()) {
//           QMessageBox::information(nullptr, "Information",
//                                               "Your comment is: <b>" + text + "</b>",
//                                               QMessageBox::Yes);
        if(!Store::getExisting_prototype_name()[window_type].contains(text)){
            //新建
            QMainWindow*  mainnew;
              if(window_type==Page_structure_utils::main){
                   mainnew = new MainWindow(text,"",nullptr);
              }
              else if(window_type==Page_structure_utils::radar){
                   mainnew = new MainWindow_Radar(text);
              }
              else if (window_type==Page_structure_utils::ecm) {
                  mainnew = new MainWindow_ECM(text);
              }
              else if (window_type==Page_structure_utils::object) {
                  mainnew = new MainWindow_object();
              }
              else if (window_type==Page_structure_utils::deploy) {
//                     mainnew = new MainWindow_object();
              }
              mainnew->setWindowTitle(text);
              mainnew->setWindowRole(QString::number(window_type));
              mainnew->setAttribute(Qt::WA_DeleteOnClose);
              if(Store::window_list.keys().contains(window_type))
                  Store::window_list[window_type].insert(text,mainnew);
              else{
                  QMap<QString,QMainWindow*> map;
                  map[text]=mainnew;
                  Store::window_list.insert(window_type,map);
              }

              return mainnew;
        }else{
              QMessageBox::warning(nullptr ,"warning", "已有同名设备存在，请新建其他名称或者导入设备。", QMessageBox::Ok);
        }
    }
    return nullptr;
}
//导入
QMainWindow* Store::open_window(Page_structure_utils::window_type window_type,QString path){
    if(path.isEmpty()) return nullptr;
    QString title = path.split("/").takeAt(path.split("/").length() - 2);
    //shao:仅仅不存在的时候才 new
    if(!Store::window_list[window_type].isEmpty()&&Store::window_list[window_type].contains(title)){
         QMessageBox::warning(nullptr ,"warning", "该设备已经存在，无需再次导入。", QMessageBox::Ok);
        return Store::window_list[window_type].value(title);
    }else{
        QMainWindow*  window;
          if(window_type==Page_structure_utils::main){
               window = new MainWindow(title, path,nullptr);
               // 如果之前未打开过，添加到文件中
       //        Utils::addProject2Pl(title, path);
          }
          else if(window_type==Page_structure_utils::radar){
               window = new MainWindow_Radar(title);
          }
          else if (window_type==Page_structure_utils::ecm) {
              window = new MainWindow_ECM(title);
          }
          else if (window_type==Page_structure_utils::object) {
              window = new MainWindow_object();
          }
          else if (window_type==Page_structure_utils::deploy) {
//                     mainnew = new MainWindow_object();
          }
        window->setToolTip(title);
        window->setWindowRole(QString::number(window_type));
        window->setWindowTitle(title);

        window->setAttribute(Qt::WA_DeleteOnClose);
        if(Store::window_list.keys().contains(window_type))
            Store::window_list[window_type].insert(title,window);
        else{
            QMap<QString,QMainWindow*> map;
            map[title]=window;
            Store::window_list.insert(window_type,map);
        }
        return window;
    }
}
//修改项目名
QString Store::edit_window_name(Page_structure_utils::window_type window_type,QString name){
    bool isOK;
    QString text = QInputDialog::getText(nullptr, "修改工程/设备名称",
                                                       "请输入新的工程/设备名",
                                                       QLineEdit::Normal,
                                                       "请不要和已有工程/设备名重复",
                                                       &isOK);
    if(isOK&&!text.isEmpty()) {
        if(Store::getExisting_prototype_name().value(window_type).contains(text)){
            QMessageBox::warning(nullptr,"名称重复","名称已经存在，请跟换新的名字再试。",QMessageBox::Ok);
            return name;
        }
        QString path=QDir::currentPath() + "/radar/"+name+"/";
        //修改文件名称
        if(QFile::exists(path+name+".rad")) Utils::modifyFileName(path,name+".rad",text+".rad");
        Utils::modifyFileName(QDir::currentPath() + "/radar/",name,text);
        //改map的key
        QMainWindow *window =Store::window_list[window_type].value(name);
        window->setWindowTitle(text);
        Store::window_list[window_type].remove(name);
        Store::window_list[window_type].insert(text,window);
       return text;
    }else{
         return name;
    }
}
//删除
bool Store::delet_window(Page_structure_utils::window_type window_type,QString name,QMainWindow* win){
    if(win!=nullptr){
        name=Store::window_list[window_type].key(win);
    }
    else{
        win=Store::window_list[window_type].value(name);
    }
    bool if_save=true;
    if(window_type==Page_structure_utils::main){
        MainWindow* window=dynamic_cast<MainWindow*>(win);
        if_save=window->isSave;
    }else if (window_type==Page_structure_utils::radar) {
        MainWindow_Radar* window=dynamic_cast<MainWindow_Radar*>(win);
        if_save=window->isSave;
    }else{
        //shao:其他类型的窗口看hi阿妹写好，就不判断保存了

    }
    if(!if_save){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, tr("关闭窗口"),
                                      "退出之前是否需要保存当前"+name+"页面数据？或者取消关闭窗口。",
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
                if(window_type==Page_structure_utils::main){
                    MainWindow* window=dynamic_cast<MainWindow*>(win);
                    if(!window->saveStatus()) return false;
                }else if (window_type==Page_structure_utils::radar) {
                    MainWindow_Radar* window=dynamic_cast<MainWindow_Radar*>(win);
                    bool a=window->save2XmlFile();
                    if(!a) return false;
                }
        }
        else if (reply == QMessageBox::No)
        {
            qDebug() << "不保存数据->退出";
        }
        else
        {
            qDebug() << "不退出";
            return false;
        }
    }
    Store::window_list[window_type].remove(name);
    return true;
}
//获取已经存在的名称
void Store::update_prototype_map(){
    // 不同目标的文件夹
    QString path_;
    QStringList namelist;
    namelist <<"project"<<"radar"<<"ecm"<<"object";
    for (int i = Page_structure_utils::main; i < namelist.size(); i++){
        path_=QDir::currentPath() + "/"+namelist.at(i)+"/";
        QList<QString> list;
        QDir dir(path_);
//        qDebug() << "各个设备的path_目录:" << path_;
        QFileInfoList fileInfoList = dir.entryInfoList();
        foreach ( QFileInfo fileInfo, fileInfoList )
        {
            if (fileInfo.fileName() == "." || fileInfo.fileName() == ".." )
                continue;
            else if ( fileInfo.isDir() )
            {
                // 将所有的name读进去
                list.append(fileInfo.fileName());
            }
        }
        //正好是Store::window_type的值加1
         Store::existing_prototype_name.insert((Page_structure_utils::window_type)i,list);
    }

}

//shao：现在没有用到了，属于一个拓展功能，所有类型的窗口整合后不太好改了
int Store::window_if_exist(Page_structure_utils::window_type window_type,QString name){
    QString window;
    bool if_exist=Store::window_list[window_type].keys().contains(name);
    if(window_type==Page_structure_utils::main){
        window="项目开发窗口";
    }else if(window_type==Page_structure_utils::radar){
        window="雷达开发窗口";
    }
    else if(window_type==Page_structure_utils::object){
        window="目标样机开发窗口";
    }
    else if(window_type==Page_structure_utils::ecm){
        window="对抗样机开发窗口";
    }
    else if(window_type==Page_structure_utils::deploy){
        window="部署管理窗口";
    }
    if (if_exist){
        QMessageBox::StandardButton rb = QMessageBox::question(nullptr, "窗口跳转", "这个名称的"+window+"已经存在，是否自动链接查看?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
                return  Store::exist_transition;
        }else {
                return  Store::exist_not_transition;
        }
    }else{
        return  Store::inexistence;
    }
}

