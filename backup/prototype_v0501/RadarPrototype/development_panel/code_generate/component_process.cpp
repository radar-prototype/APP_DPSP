#include "component_process.h"
#include <QDebug>
#include "utils.h"
#include "codewindow.h"
// This is an auto-generated comment for this class.
/********************************************************************************************
 * @author Antrn
 * @motto Done is better than Perfect, Talk is cheap. Show me the code.
 * @date 2020-10-15 T 16:15:15
 * @description 组件进程处理类
 * @copyright (c)
 ********************************************************************************************/
Component_process::Component_process(QObject *parent) : QObject(parent)
{

}
// This is a comment generated automatically by the system when antrn reviews other people's code.
/**
 * @author
 * @addDate 2020-10-15 T 16:22:16
 * @wantosay FIXME 待修改，这函数参数目前都没用到
 * @brief 编辑场景中算法组件的代码
 * @param item 未知
 * @param gitem 未知
 */
void Component_process::codeEdit(QListWidgetItem *item, QGraphicsItem*gitem, DiagramItem *ditem, AlgorithmComp* ac)
{
    Q_UNUSED(item)
    // 将当前场景中的组件传入，编辑它的代码
    CodeWindow *cw = new CodeWindow(nullptr, nullptr, gitem);
    QList<QString> codeList;
    //shao：关于线程数： 按原先李总给的代码这里的总线程数是先设置好的，（比实际这个组件启动的线程数大）所以在整体设置中设置了，但是现在我觉得没必要了，所以就直接用组件参数的了
    //后面有需要在修改吧，因为设置时间的问题，还需要一套信号槽单独控制
    codeList.append(this->get_h_code(ac, ditem, 8));
    codeList.append(this->get_cpp_code(ac, ditem));
    codeList.append(this->get_txt(ditem->getIconName(), ditem->callFunction));
    // Antrn: FIXME  shao: 1
    cw->component_code_input(codeList, ditem);
    cw->show();
    //shao:重新生成代码
    connect(cw, &CodeWindow::regenerate, this, [cw, codeList, ditem]()
    {
        //存代码的目录
        QString code_dir_name = QDir::currentPath() + "/radar/" + ditem->getRadar_id() + "/room/codes/" + ditem->getIconName() + ditem->itemSuuid.mid(1, ditem->itemSuuid.size() - 2) + "/";
        Utils::deleteFileOrFolder(code_dir_name);
        cw->close();
        cw->component_code_input(codeList, ditem);
        cw->show();
    });
}

void Component_process::parameter_configuration(AlgorithmComp algorithmComp, DiagramItem *gitem)
{
    this->algorithmComp = algorithmComp;
    this-> component_info_dialog = new Component_info_dialog();
    this->component_info_dialog->setFlag(1);//flag=1:是参数设置不是默认参数设置 和组件管理部分区分来

    this->component_info_dialog->edit_dialog(&algorithmComp);

    connect(component_info_dialog, &Component_info_dialog::edit_succeed, this, [this, gitem](AlgorithmComp * ac)
    {
        //用不到的后面再删除
        this->algorithmComp = *ac;
//        qDebug()<<"1111111111111111:"<<ac->getParam().value("a").value("value");
        emit edit_succeed(ac);
        //存代码的目录
        QString code_dir_name = QDir::currentPath() + "/radar/" + gitem->getRadar_id() + "/room/codes/" + gitem->getIconName() + gitem->itemSuuid.mid(1, gitem->itemSuuid.size() - 2) + "/";
        Utils::deleteFileOrFolder(code_dir_name);
    });
}

// Antrn: FIXME 未完成，烂尾楼
QMap<QString, QMap<QString, QString>> Component_process::get_parameter_value(AlgorithmComp* algorithmComp)
{
    //shao: 留下来函数，将来写一些处理参数的代码 （例如 对自定义类型代码的修改，用不到就算了）
    return algorithmComp->getParam();
}

QString Component_process::get_parameter_init_code(AlgorithmComp* algorithmComp)
{
    QString init_code = "";
    QMap<QString, QMap<QString, QString>> pMap = this->get_parameter_value(algorithmComp);
    QMap<QString, QMap<QString, QString>>::Iterator comp;
    for ( comp = pMap.begin(); comp != pMap.end(); ++comp )
    {
        if(QString::compare(comp.value().value("isoutput"), "2") == 0)
        {
            //根据类型写出不同的初始化语句 这里直接用=了
            init_code += "pCom_" + algorithmComp->getFileName().toLower() + "." + comp.key() + " = " + comp.value().value("value") + ";\n\t";
        }
    }
    return init_code;
}

QString Component_process::get_ip_init_code(DiagramItem* diagramItem)
{
    QString lower_name = diagramItem->getIconName().toLower();
    QString writeCont = "component_name = \"" + lower_name + "\";\n\t" +
                        "component_ip = \"" + diagramItem->getIn_whichIP() + "\";\n\t";
    return writeCont;
}
// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-11-12 T 14:46:56
 * @copyright (c)
 * @brief 为一个组件生成h文件
 * @param algorithmComp 算法组件
 * @param diagramItem 一个场景中的图元对象
 * @param thread_num 线程数
 * @return
 */
QString Component_process::get_h_code(AlgorithmComp* algorithmComp, DiagramItem * diagramItem, int thread_num)
{
    //shao： run中以后还有器他参数传入的话，建议换成结构体
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString code = R"(#ifndef SIMULATE_H
#define SIMULATE_H
#include "Common.h"
#include "MsgQueue.h"
class Simulate : public Common
{
public:
    // 组件属性
    RSPL_NAME_ICOMP pCom_[lower];
    // 组件接收线程
    pthread_t tidp_[lower];
    // 组件处理线程
    pthread_t tidp_[lower]_thread[THREAD_NUM];
    // 组件的名字
    string component_name;
    // 组件的ip
    string component_ip;

    QUEUE

    // 构造
    Simulate();

    void init();

    void run(int *thread_id);

    void uninit();

    void join();

    FUNCTION
};
#endif /* SIMULATE_H */
)";
    QString name = diagramItem->getIconName();
    QString fullName = diagramItem->callFunction;
    code.replace("[lower]", name.toLower());
    code.replace("SIMULATE", name.toUpper());
    code.replace("NAME", name.toUpper());
    code.replace("Simulate", Utils::toCapital(name));
    code.replace("THREAD_NUM", QString::number(thread_num));
//    根据端口数来确定队列和函数个数
    QString code_queue = "";
    QString code_function = "";
    int number_output = 1;
    int number_input = 1;
    QMap<QString, QMap<QString, QString>> pMap = this->get_parameter_value(algorithmComp);
    QMap<QString, QMap<QString, QString>>::Iterator comp;
    for ( comp = pMap.begin(); comp != pMap.end(); ++comp )
    {
        if(QString::compare(comp.value().value("isoutput"), "2") != 0)
        {
            if(QString::compare(comp.value().value("isoutput"), "0") == 0)
            {
                code_queue += "static MSG_Q_ID queue_receive_" + QString::number(number_output) + ";//组件接收队列 --接收线程负责将数据取出放到内部队列  \n\t";
                code_function += "static void* fun_receive_thread_" + QString::number(number_output) + "(void* arg);//用户自己配置  \n\t";
                number_output++;
            }
            if(QString::compare(comp.value().value("isoutput"), "1") == 0)
            {
                code_queue += "//参数thread_id:检测线程顺序\n\t"
                              "static MSG_Q_ID queue_handle_" + QString::number(number_input) + ";//组件结果暂存队列 --处理线程负责将数据取出处理后发送到这个队列 \n\t";
                code_function += "static void* fun_handle_" + QString::number(number_input) + "(void* arg);//用户自己配置 \n\t";
                number_input++;
            }
        }
    }
    code.replace("QUEUE", code_queue);
    code.replace("FUNCTION", code_function);
    return code;
}

//FIXME: 队列长度和大小还没有改用中体配置的参数  msgQCreate(&(Simulate::gMsgQPre), 20, 41376, MSG_Q_FIFO);
QString Component_process::get_cpp_code(AlgorithmComp* algorithmComp, DiagramItem * diagramItem)
{
    QString name = diagramItem->getIconName();
    QString fullName = diagramItem->callFunction;
    QString code = R"(#include "simulate.h"

// 初始化队列
QUEUE

// 构造函数
Simulate::Simulate(void){

}

//初始化函数来源于算法组件的参数配置和集群配置（用户可根据需要修改）
void Simulate::init(){
    // 参数初始化赋值
    parameter_init_code
    // 集群相关初始化赋值
    ip_init_code
    // 队列初始化后半部分，注意修改队列大小
    QU_INIT
}

// 运行函数
void Simulate::run(int *thread_id){
    // 创建线程
    THREAD
}

void Simulate::join(){
    // 把该组件的join写进去
    JOINS
}
// 请修改以下函数的实际内容 注意队列命名
FUNCTION
)";
    //FIXME 这里应该有 根据端口的循环拼接
    QString code_queue = "";
    QString code_queue_init = "";
    QString code_function = "";
    QString code_thread = "";
    QString code_pthread_join = "";
    int number_output = 1;
    int number_input = 1;
    QMap<QString, QMap<QString, QString>> pMap = this->get_parameter_value(algorithmComp);
    QMap<QString, QMap<QString, QString>>::Iterator comp;
    for ( comp = pMap.begin(); comp != pMap.end(); ++comp )
    {
        if(QString::compare(comp.value().value("isoutput"), "2") != 0)
        {
            // Antrn: 接收端口
            if(QString::compare(comp.value().value("isoutput"), "0") == 0)
            {
                code_queue += "MSG_Q_ID Simulate::queue_receive_" + QString::number(number_output) + ";//接收队列\n";

                code_queue_init += "msgQCreate(&(Simulate::queue_receive_" + QString::number(number_output) + "), 20, QU_LEN, MSG_Q_FIFO);\n\t";

                code_function += "void* Simulate::fun_receive_thread_" + QString::number(number_output) + "(void* arg){//用户填写函数内的代码\n\t\n}  \n";

                code_thread += "\tpthread_create(&tidp_"+name.toLower()+", NULL, Simulate::fun_receive_thread_" + QString::number(number_output) + ",(void*)&(pCom_" + name.toLower() + "));\n\t";
                code_pthread_join += "\tpthread_join(tidp_" + name.toLower() + ", NULL);\n";
                number_output++;
            }

// Antrn: 输出端口
if(QString::compare(comp.value().value("isoutput"), "1") == 0)
{

    code_queue += "MSG_Q_ID Simulate::queue_handle_" + QString::number(number_input) + ";//内部队列\n";

    code_queue_init += "msgQCreate(&(Simulate::queue_handle_" + QString::number(number_input) + "), 20, QU_LEN, MSG_Q_FIFO);\n\t";

    code_function += "void* Simulate::fun_handle_" + QString::number(number_input) + "(void* arg){//用户填写函数内的代码\n\t\n}  \n";
    //仅仅SIM不一样
    if(QString::compare(name.toLower(), "sim") == 0)
    {
        code_thread += R"(// 默认处理函数循环
    pthread_create(&tidp_)" + name.toLower() + R"(, NULL,Simulate::fun_handle_)" + QString::number(number_input) + R"(, (void *)&(pCom_)" + name.toLower() + R"());
)";
        code_pthread_join += "\tpthread_join(tidp_" + name.toLower() + ", NULL);\n";
        // 这块注意修改log文件的位置，用户应该自己写到任意一个组件的init中
        code_queue_init += "rspl_initialize(RLL_INFO, RLT_CONSOLE, \"files/rspl.log\");\n\t";
    }
    else
    {
        code_thread += R"(
    // 默认处理函数循环
    for (int i = 0; i < pCom_)" + name.toLower() + R"(.num_thread; i++)
    {
        thread_id[i] = i;
        pthread_create(&tidp_)" + name.toLower() + R"(_thread[i], NULL,Simulate::fun_handle_)" + QString::number(number_input) + R"(, (void *)&thread_id[i]);
    }
)";
    code_pthread_join += R"(
    for (int i = 0; i < pCom_)" + name.toLower() + R"(.num_thread; i++)
    {
        pthread_join(tidp_)" + name.toLower() + R"(_thread[i], NULL);
    }
)";
    }
        number_input++;
            }
        }
    }

    code.replace("QUEUE", code_queue);
    code.replace("QU_INIT", code_queue_init);
    code.replace("FUNCTION", code_function);
    code.replace("JOINS", code_pthread_join);
    code.replace("THREAD", code_thread);

    //shao：注意必须在 code.replace("QUEUE",code_queue); 那之后替换
    QString code_queue_lenght = this->get_queue_len();
    code.replace("QU_LEN", code_queue_lenght);

    code.replace("NAME", name.toUpper());
    code.replace("simulate", name.toLower());
    code.replace("Simulate", Utils::toCapital(name));
    code.replace("parameter_init_code", this->get_parameter_init_code(algorithmComp));
    code.replace("ip_init_code", this->get_ip_init_code(diagramItem));
    code.replace("Simulate", Utils::toCapital(name));
//    code.replace("THREAD_NUM",QString::number(thread_num));

    return code;
}

//FIXME:待完善细节
QString Component_process::get_txt(QString name, QString fullName)
{
    QString txt = R"(/**
 * 这里为用户提供，用来记录代码之外的附加信息//
 * 类：Simulate
 * 代码修改时间：time
 **/
)";
    txt.replace("time", QDate::currentDate().toString()+" "+QTime::currentTime().toString());
    txt.replace("NAME", name.toUpper());
    txt.replace("Simulate", Utils::toCapital(fullName));
    return txt;
}

QString Component_process::get_queue_len()
{
    QString code_queue_lenght = "sizeof(rspl_thread_param)";
    return code_queue_lenght;
}
