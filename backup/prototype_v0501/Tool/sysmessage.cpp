#include "sysmessage.h"
#include <qfile.h>
#include <qdir.h>
#include <stdio.h>
#include <stdlib.h>
#include <qdebug.h>
#include <QDate>
#include <qtimer.h>

/* Global variable
 * 全局变量可以为本文件中的其他函数所共用，它的有效范围为从定义变量的位置开始到本源文件结束。
 */
static char *buf_tmp;
static SysMessage *message;
QTextEdit *edit_message;

SysMessage::SysMessage()
{
    message_buf_size = 10240;
    message_buf_A = new char[10240];
    message_buf_B = new char[10240];
    buf_tmp       = new char[1024];
    buf_offset = 0;
    new_buf = old_buf = message_buf_A;
    buf_A_B = false; // A
}

SysMessage::~SysMessage()
{
    delete message_buf_A;
    delete message_buf_B;
    delete buf_tmp;
}

void SysMessage::open_sys_message(QTextEdit *e)
{
    edit_message = e;
    if (message == nullptr) message = new SysMessage;
}

void SysMessage::open_sys_message_without_textEdit()
{
    if (message == nullptr) message = new SysMessage;
}

void SysMessage::close_sys_message()
{
    if (message != nullptr)
    {
        message->write_message();
        edit_message = nullptr;
        delete message;
    }
}

void SysMessage::SysMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QDate date = QDate::currentDate();
    QString str_date = QString("%1-%2-%3-%4").arg(date.year()).arg(date.month()).arg(date.day()).arg(QTime::currentTime().toString());
    if(edit_message)
        edit_message->append(str_date + QString(": ") + msg);

    switch (type)
    {
    case QtDebugMsg:
        sprintf(buf_tmp, "[File:(%s:%d) Fun:(%s) Date:(%s)] Debug: %s\n",
                context.file, context.line, context.function, str_date.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
        break;

    case QtInfoMsg:
        sprintf(buf_tmp, "[File:(%s:%d) Fun:(%s) Date:(%s)] Info: %s\n",
                context.file, context.line, context.function, str_date.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
        break;

    case QtWarningMsg:
        sprintf(buf_tmp, "[File:(%s:%d) Fun:(%s) Date:(%s)] Warning: %s\n",
                context.file, context.line, context.function, str_date.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
        break;

    case QtCriticalMsg:
        sprintf(buf_tmp, "[File:(%s:%d) Fun:(%s) Date:(%s)] Critical: %s\n",
                context.file, context.line, context.function, str_date.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
        break;

    case QtFatalMsg:
        sprintf(buf_tmp, "[File:(%s:%d) Fun:(%s) Date:(%s)] Fatal: %s\n",
                context.file, context.line, context.function, str_date.toLocal8Bit().constData(), msg.toLocal8Bit().constData());
        break;//abort();
    }
    message->write_message();
}


void SysMessage::run()
{
    QDate   date = QDate::currentDate();
    QString file_name = QString("E:\\Desktop\\%1-%2-%3%4").arg(date.year()).arg(date.month()).arg(date.day()).arg(QString(".log"));
    QFile message_file(file_name);
    if(!message_file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "Open failed.";
        return;
    }
    message_file.write(old_buf);
    message_file.close();
}

void SysMessage::write_message()
{
    int len = strlen(buf_tmp);
    strcpy(new_buf + buf_offset, buf_tmp);
    buf_offset += len;
    if ((buf_offset + 1024) > message_buf_size)
    {
        this->start(QThread::LowestPriority);
        buf_offset = 0;
        old_buf = new_buf;
        new_buf = (buf_A_B ? message_buf_B : message_buf_A);
        buf_A_B = !buf_A_B;
    }
}
