#include "codepreview.h"
#include "ui_codepreview.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QDebug>

CodePreview::CodePreview(QString fn, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CodePreview),
    codeFileName(fn)
{
    ui->setupUi(this);
    ui->codeEdit->installEventFilter(this);
    ui->codeEdit->setTabStopWidth(40);
    ui->codeEdit->setMinimumSize(800, 600);
    if(!loadFile(codeFileName))
    {
        QMessageBox::warning(nullptr, "错误", "加载代码出错！");
    }
    else
    {
        ui->codeEdit->showNormal();
    }
}

bool CodePreview::eventFilter(QObject *widget, QEvent *event)
{
    if(widget == ui->codeEdit)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            // Ctrl+S保存操作
            if (ev->key() == Qt::Key_S && (ev->modifiers() & Qt::ControlModifier))
            {
                if(ui->codeEdit->document()->isModified())
                {
                    save();
                    // return true 说明处理完毕，false就是得继续下一步进行处理
                    return true;
                }
            }
        }
    }
    return false;
}

CodePreview::~CodePreview()
{
    delete ui;
}

bool CodePreview::save()
{
    // 新建文件对象
    QFile file(codeFileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("多代码编辑器"), tr("无法写入文件 %1:\n%2.").arg(codeFileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->codeEdit->toPlainText();

    QApplication::restoreOverrideCursor();
    // 文件内容已经保存
    setCodeFileName(codeFileName);
    file.close();
    return true;
}

bool CodePreview::loadFile(QString fpath)
{
    QFile file(fpath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "文件打开出错！~";
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->codeEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    file.close();
    // 加载完文件之后，文件内容是认为未修改
    setCodeFileName(fpath);
    connect(ui->codeEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    return true;
}

QString CodePreview::userFriendlyCurrentFile()
{
    // 从文见路径中提取文件名
    return QFileInfo(codeFileName).baseName();
}

void CodePreview::documentWasModified()
{
    //设置文档的属性isModified
    setWindowModified(ui->codeEdit->document()->isModified());
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

void CodePreview::closeEvent(QCloseEvent *event)
{
    // 关闭前检查，如果返回true，则关闭窗口
    if (maybeSave())
    {
        event->accept();
        deleteLater();
    }
    else       // 否则忽略该事件
    {
        event->ignore();
    }
}

bool CodePreview::maybeSave()
{
    if (ui->codeEdit->document()->isModified())   // 如果代码被更改过
    {
        QMessageBox box;
        box.setWindowTitle(tr("代码编辑器"));
        box.setText(tr("是否保存对“%1”的更改？")
                    .arg(userFriendlyCurrentFile()));
        box.setIcon(QMessageBox::Warning);

        // 添加按钮，QMessageBox::YesRole可以表明这个按钮的行为
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"), QMessageBox::YesRole);
        box.addButton(tr("否(&N)"), QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("取消"), QMessageBox::RejectRole);
        box.exec(); // 弹出对话框，让用户选择是否保存修改，或者取消关闭操作
        if (box.clickedButton() == yesBtn)  // 如果用户选择是，则返回保存操作的结果
            return save();
        else if (box.clickedButton() == cancelBtn) // 如果选择取消，则返回false
            return false;
    }
    return true; // 如果代码没有更改过，则直接返回true
}

QString CodePreview::getCodeFileName() const
{
    return codeFileName;
}

void CodePreview::setCodeFileName(const QString &value)
{
    codeFileName = value;
    ui->codeEdit->document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}
