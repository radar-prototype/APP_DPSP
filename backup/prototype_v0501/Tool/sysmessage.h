#ifndef SYSMESSAGE_H
#define SYSMESSAGE_H

#include <qthread.h>
#include <qtextedit.h>

/**
 * @brief The SysMessage class 系统日志生成类（游离于主进程之外的   / 方法：内存拷贝）
 */
class SysMessage : public QThread
{
public:
    SysMessage();
    ~SysMessage();
    void run();
    void write_message();
    /*  Out message to file and UI.
     *
     */
    static void open_sys_message(QTextEdit *e);
    static void open_sys_message_without_textEdit();
    static void close_sys_message();
    static void SysMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    unsigned int message_buf_size;
    char        *message_buf_A; //take turns using buf
    char        *message_buf_B;
    char        *new_buf;
    char        *old_buf;
    bool         buf_A_B; //false: A   true:B

    unsigned int buf_offset;

    QString date;
};

#endif // SYSMESSAGE_H
