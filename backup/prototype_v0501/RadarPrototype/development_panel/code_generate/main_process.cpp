#include "main_process.h"
#include "main_config_dialog.h"
#include "utils.h"
#include <QStringList>
// This is an auto-generated comment by Antrn.
/********************************************************************************************
 * @author Antrn
 * @motto Done is better than Perfect, Talk is cheap. Show me the code.
 * @date 2020-10-15 T 15:52:26
 * @description shao：用来处理代码生成过程中所有主流程（整个的代码生成过程，区别于和组件相关的子模块）相关操作都在这里
 * @copyright (c)
 ********************************************************************************************/
Main_process::Main_process(QObject *parent) : QObject(parent)
{
    this->component_process = new Component_process(this);
//    弹窗询问是否设置好了组件? mao: what? Just DO it！
}

//shao:配置主窗口的设置项
void Main_process::Main_function_configuration(QMap<QString, AlgorithmComp> pMap)
{
    this->main_config_dialog = new Main_config_dialog();
    main_config_dialog->show();
    if(main_config_map.empty())
    {
        QMap<QString, QString> map;
        // Antrn: 暂时存放每个组件的参数map
        QMap<QString, QMap<QString, QString>> param_map;
        // Antrn: 默认的最大线程数，后面遍历更新
        int max_thread = 10;
        QMap<QString, AlgorithmComp>::Iterator comp;
        QString tempname;
        // Antrn: 在map中主要insert:组件名，该组件的线程数量
        for ( comp = pMap.begin(); comp != pMap.end(); ++comp )
        {
            tempname = comp->getFileName();
            if(tempname.compare("display") == 0 || tempname.compare("INPUT") == 0 || tempname.compare("OUTPUT") == 0)
            {
                qDebug() << "输入输出和显示组件不统计线程数";
            }
            else
            {
                // 计算map的默认值，最小值
                param_map = comp->getParam();
                QString  val = "10";
                if(param_map.contains("num_thread")) val = param_map.value("num_thread").value("value");
                map.insert(tempname, val);
                if(val.toInt() > max_thread) max_thread = val.toInt();
            }
        }
        // Antrn: 插入其他属性
        map.insert("file_path", "files/rspl.log");
        map.insert("max_thread", QString::number(max_thread)); //应该用最大的组件进程数
        // Antrn: 数据队列长度
        map.insert("msg_len", "20");
        // Antrn: 队列空间大小  QUES 什么作用？
        map.insert("msg_size", "41376");
        main_config_dialog->set_default_value(map);
    }
    else
    {
        main_config_dialog->set_default_value(main_config_map);
    }

    //配置完成的时候触发
    connect(main_config_dialog, &Main_config_dialog::setup_complete, this, [&]()
    {
        this->main_config_map = main_config_dialog->get_parameter_value();
        main_config_dialog->close();
    });
}

/**
 * @brief 生成main.c代码文件
 * @param dlist 场景中的组件列表，应该不包括输入输出和显示组件
 * @param mainCppPath main代码生成所在的路径
 * @return 是否生成成功
 */
bool Main_process::generate_main_code(QList<DiagramItem *> *dlist, QString mainCppPath)
{
    // 先包括头文件
    QString code = "#include \"Common.h\"";
    QString code_h = R"(
#include "simulate.h")";
    for ( DiagramItem* diagramItem : *dlist )
    {
        QString temp = code_h;
        temp.replace("simulate",  diagramItem->getIconName().toLower());
        code += temp;
    }
    //shao:之后总体参数配置添加的地方
    code += R"(
int main()
{
    // 总体参数配置 thread_id:检测线程顺序
    int i, thread_id[10];
    for(i=0; i<10; i++)  thread_id[i] = i;
)";
    QMap<QString, AlgorithmComp>::Iterator comp;
    for ( DiagramItem* diagramItem : *dlist )
    {
        QString code_comp = R"(
    Simulate simulate;
    simulate.init();
    simulate.run(thread_id);
)";
        code_comp.replace("simulate", diagramItem->getIconName().toLower());
        code_comp.replace("Simulate",  Utils::toCapital(diagramItem->getIconName()));
        code += code_comp;
    }
    code += "\n";
    for ( DiagramItem* diagramItem : *dlist )
    {
        QString joinCode = "\tsimulate.join();\n";
        joinCode.replace("simulate", diagramItem->getIconName().toLower());
        code += joinCode;
    }
    code += R"(
    return 0;
}
)";
    QString radar_code_file = mainCppPath + "main.cpp";
    QFile writerf(radar_code_file);
    // 流读取文件
    QTextStream tscode(&writerf);
    tscode.setCodec(QTextCodec::codecForName("utf-8"));
    if(!writerf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed, Check what's wrong1.";
        return false;
    }
    //写入文件
    tscode << code << endl;
    writerf.close();
    return true;
}

//shao:代码生成
/**
 * @brief Main_process::code_generate　分两部生成需要的代码，最后打包
 * @param dlist 不包括输入输出和显示组件的组件列表
 * @param mainCodePath　生成main的代码文件的路径
 * @return　是否生成成功
 */
bool Main_process::code_generate(QList<DiagramItem *> *dlist, QString mainCodePath)
{
    // 生成main.c
    bool is_main_succ = this->generate_main_code(dlist, mainCodePath);
    //　生成makefile
    bool is_makefile_succ = this->generate_makefile(mainCodePath);
    // 将当前场景中的组件传入，编辑它的代码 其实不用编辑 所以可以先不显示 看后面需求
//    CodeWindow *cw = new CodeWindow();
//    cw->main_code_disply(mainCodePath);
    // 打开代码显示窗口 FIXME: cw中的修改工作量不小 先不看了 Antrn:???得看
//    cw->show();
    return is_main_succ && is_makefile_succ;
}

//　shao: 生成makefile
bool Main_process::generate_makefile(QString mainCppPath)
{
    // 这个模板有问题
#if 0
    // Antrn: 通用makefile模板
    QString makefile = R"(CC:= g++
TARGET:= main

CFLAGS:= -g -Wall -D_REENTRANT

LIBS:= -L  /lib/ -lvsip -lsubscriber -lpublisher -lrspl -lvsip -lpthread

#OBJECTS := MsgQueue.o  sim.o dpc.o cfar.o mtd.o sdae.o  slc.o main.o



SRCS :=$(wildcard *.cpp)

OBJECTS := $(patsubst %cpp,%o,$(SRCS))

$(TARGET): $(OBJECTS)
   $(CC) -o $(TARGET)  $(OBJECTS) $(LIBS)

# $@表示所有目标集
%.o:%cpp
   $(CC) -c $(CFLAGS)  $< -o $@


.PHONY : clean
clean:
   -rm -f $(OBJECTS) $(TARGET)
)";
    // Antrn: makefile路径
    QString path_makefile = mainCppPath + "makefile";
    QFile writerf(path_makefile);
    // 流读取文件
    QTextStream tscode(&writerf);
    tscode.setCodec(QTextCodec::codecForName("utf-8"));
    if(!writerf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed, Check what's wrong1.";
        return false;
    }
    //写入文件
    tscode << makefile  << endl;
    writerf.close();
#endif
    QString path_makefile = mainCppPath + "makefile";
    return Utils::copyFileToPath(QDir::currentPath()+"/codes/makefile", path_makefile, true);
}

//shao: 打包
/**
 * @brief 把已经生成的各个组件的代码复制到target目录中
 * @param dlist 算法组件列表
 * @param mainCppPath　生成main.c文件的路径
 * @return
 */
QString Main_process::package(QList<DiagramItem *> *dlist, QString mainCppPath)
{
    QString targetpath = mainCppPath + "target/";
    // Antrn: 创建target文件夹
    Utils::openDirOrCreate(targetpath);//shoa:比较懒，仅仅为了创建
    //copy代码
    for ( DiagramItem* ditem : *dlist )
    {
        QString code_dir_name = QDir::currentPath() + "/radar/" + ditem->getRadar_id() + "/room/codes/" + ditem->getIconName() + ditem->itemSuuid.mid(1, ditem->itemSuuid.size() - 2) + "/";
        QStringList fileList = Utils::getDirFileList(code_dir_name);
        // fileList大小不为０说明用户已经编辑过代码
        for (int i = 0; i < fileList.size(); ++i)
        {
            QString filePath = fileList.at(i);
            qDebug() << filePath;
            Utils::copyFileToPath(code_dir_name + filePath, targetpath + filePath, true);
        }
//        Utils::copyDirectoryFiles(code_dir_name,targetpath,true);//复制整个文件夹  好像没用
//        Utils::deleteFileOrFolder(code_dir_name);
    }
    Utils::copyFileToPath(mainCppPath + "main.cpp", targetpath + "main.cpp", true);
    Utils::copyFileToPath(mainCppPath + "makefile", targetpath + "makefile", true);
    //打包文件
    QProcess * p = new QProcess(this);
    p->setWorkingDirectory(mainCppPath);//设置工作目录
    p->start("tar -cvzf target.tar.gz ./target" );//最后的参数必须是 ./target  如果是一个完整 这个路劲也会被打包就去
    while (false == p->waitForFinished(1000)) { ;}
    p->close();
    bool isCopy = Utils::copyDirectoryFiles(mainCppPath+"target",QDir::currentPath()+"/target",true);
    qDebug() << "暂时先将target复制到根目录，便于王源的RPC传输文件后解压不那么深，成功？" << isCopy;
    return mainCppPath + "target.tar.gz";
}

Component_process *Main_process::getComponent_process() const
{
    return component_process;
}
