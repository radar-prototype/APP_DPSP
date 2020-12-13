#ifndef MAIN_PROCESS_H
#define MAIN_PROCESS_H

#include <QObject>
#include "algorithmcomp.h"
#include "component_process.h"
#include "main_config_dialog.h"

class Main_process : public QObject
{
    Q_OBJECT
public:
    explicit Main_process(QObject *parent = nullptr);
    //shao:配置主窗口的设置项
    void Main_function_configuration(QMap<QString, AlgorithmComp> pMap);
    //shao: main函数代码生成
    bool generate_main_code(QList<DiagramItem *> *dlist, QString mainCppPath);
    //shao:代码生成
    bool code_generate(QList<DiagramItem *> *dlist, QString radar_code_file);
    //shao: 生成makefile
    bool generate_makefile(QString path_makefile);
    //shao: 打包
    QString package(QList<DiagramItem *> *dlist, QString mainCppPath);

    Component_process *getComponent_process() const;



signals:

public slots:

private:
    Main_config_dialog* main_config_dialog;
    //用来操作子组件
    Component_process*  component_process;
    //shao:存主函数配置
    QMap<QString, QString> main_config_map; //不可以是指针的

    QString code_template_start = "#include \"RspMsgQueue.h\"\n";

    QString declareMid = "";

    QString Simulate_Component = "void *Simulate_Component(void *arg)\n"
                                 "{\n\twhile (1)\n\t{\n\t\tchar *p = \"aaaaaaaaaaaaa\";\n"
                                 "\t\tmsgQSendNext(&gMsg[_AFTER_FIRST_]DatQ, p, 15, 0, 1, &param_sim, &param_dpc);\n\t\tusleep(200);\n\t}\n}\n";

    QString fun_component = "void *[_COMPONENT_NAME_]_Component(void *arg)\n"
                            "{\n\twhile (1)\n\t{\n\t\t"
                            "msgQReceivePre(&gMsg[_GMSGQ_LAST_]DatQ, p, 15, 0, &param_[_LOWERCASE_LAST_], &param_[_LOWERCASE_THIS_]);\n"
                            "\t\tmsgQSend(&gMsg[_GMSGQ_THIS_]Q, p, 15, 0, 1);\n"
                            "\t\tfree(p);\n\t}\n}\n";

    QString fun_thread = "void *[_COMPONENT_NAME_]_thread(void *arg)\n"
                         "{\n\twhile (1)\n\t{\n\t\tchar *p = (char *)malloc(15 * sizeof(char));\n"
                         "\t\tmsgQReceive(&gMsg[_GMSGQ_THIS_]Q, p, 15, 0);\n"
                         "\t\tmsgQSendNext(&gMsg[_GMSGQ_THIS_]DatQ, p, 15, 0, 1, &param_[_LOWERCASE_THIS_], &param_[_LOWERCASE_NEXT_]);\n"
                         "\t\tfree(p);\n\t}\n\treturn NULL;\n}\n";

    QString main_params_init = "\nint main(int arg, char *argv[]){\n"
                               "\tint i, thread_id[_THREAD_ALL_];\n";

//    QString declareMid = "\tstruct timeval t1, t2, t3;\n\t//实验计时用//time\n"
//                         "\tint i = 0;\n"
//                         "\tdouble count = 0.0; //time\n"
//                         "\tint number = 4; //数据的组数，一组为9600\n";

    QString code_template_end = "}\n";
};

#endif //MAIN_PROCESS_H
