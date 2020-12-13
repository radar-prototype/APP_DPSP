#include "algocodeedit.h"
#include "linenumberarea.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <QFontMetricsF>
#include <QPainter>
#include <QTextBlock>
#include <QDebug>

AlgoCodeEdit::AlgoCodeEdit(QWidget *parent):
    QPlainTextEdit(parent)
{
    //子窗口关闭时销毁这个类的对象
    setAttribute(Qt::WA_DeleteOnClose);
    font = new QFont("Courier", 12, QFont::Medium);
    this->setFont(*font);
    // 行号区域的Widget
    lineNumberArea = new LineNumberArea(this);
    //每当按下回车或者删除回车(更新字符块),则newBlockCount计数,并触发该信号, newBlockCount 默认为1
    connect(this, &AlgoCodeEdit::blockCountChanged, this, &AlgoCodeEdit::updateLineNumberAreaWidth);
    connect(this, &AlgoCodeEdit::updateRequest, this, &AlgoCodeEdit::updateLineNumberArea);
    //光标改变时发出信号
    connect(this, &AlgoCodeEdit::cursorPositionChanged, this, &AlgoCodeEdit::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    //增加文本关键词高亮：char
    mshlighter = new MySyntaxHighlighter(this->document());
}

#if 0
/**
 * 新建文件
 * @brief AlgoCodeEdit::newFile
 */
void AlgoCodeEdit::newFile(const QString &fileName)
{
    //未被保存(重命名)过
    isUntitled = true;
    curFile = fileName;
    //设置窗口标题，加×号
    setWindowTitle(curFile+"[*]"+tr("-文档编辑器"));
    //文档更改时发射contentsChanged信号，执行documentWasModified槽
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

bool AlgoCodeEdit::loadFile(const QString &fileName)
{
    //新建QFile对象
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        // 在主窗口中间显示
//        int x =this->x() + (this->width() - aboutdialog.width()) / 2;
//        int y =this->y() + (this->height() - aboutdialog.height()) / 2;
        QMessageBox::warning(this, tr("文档编辑器"),tr("无法读取文件%1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    return true;
}

bool AlgoCodeEdit::save()
{
    //如果未被保存过，执行另存为
    if(isUntitled){
        return saveAs();
    }else{
        return saveFile(curFile);
    }
}

bool AlgoCodeEdit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), curFile);
    //获取文件路径，为空返回false，否则保存
    if(fileName.isEmpty()){
        return false;
    }else{
        return saveFile(fileName);
    }
}

bool AlgoCodeEdit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("多文档编辑器"), tr("无法写入文件 %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    return true;
}

QString AlgoCodeEdit::userFriendlyCurrentFile()
{
    //从文见路径中提取文件名
    return QFileInfo(curFile).fileName();
}

#endif

/**
 * 绘图事件，每次更新重绘行号区域
 * @brief AlgoCodeEdit::lineNumberAreaPaintEvent
 * @param event
 */
void AlgoCodeEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(),QColor(240,240,240));
    QTextBlock block = firstVisibleBlock();
    // 行数
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::gray);
            // 设置行号的字体，默认是SansSerif，12号
            painter.setFont(*font);
            // 居中显示
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignCenter, number);
        }

        // 下一行
        block = block.next();
        // 上一行的底部变为下一行的顶部
        top = bottom;
        // 更新bottom的高度，加上每一行block的高度
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }

}

/**
 * 通过计算行数，从而确定最大行号的位数
 * @brief AlgoCodeEdit::lineNumberAreaWidth
 * @return
 */
int AlgoCodeEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    // 几位数字
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 20 + fontMetrics().width((QLatin1Char('9'))) * digits;

    return space;
}

#if 0
//void AlgoCodeEdit::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier))
//    {
//        if(document()->isModified()){
//            save();
//        }
//    }
//    if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ControlModifier))
//    {
//        if(document()->isModified()){
//            save();
//        }
//    }
//}

#endif
/**
 * 窗口改变大小的时候
 * @brief AlgoCodeEdit::resizeEvent
 * @param event
 */
void AlgoCodeEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    // 更新行号区域的坐标
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));

}

//void AlgoCodeEdit::documentWasModified()
//{
//    //设置文档的属性isModified
//    setWindowModified(document()->isModified());
//}

/**
 * 更新一下margin
 * @brief AlgoCodeEdit::updateLineNumberAreaWidth
 */
void AlgoCodeEdit::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

/**
 * 当鼠标光标在某一行就高亮显示那一行
 * @brief AlgoCodeEdit::highlightCurrentLine
 */
void AlgoCodeEdit::highlightCurrentLine()
{

    QList<QTextEdit::ExtraSelection> extraSelections;
    // 是可编辑的
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        // 黄色高亮
        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        // 整行选中
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

/**
 * @brief AlgoCodeEdit::updateLineNumberArea
 * @param rect 长方形区域
 * @param dy 向下滚动的距离
 */
void AlgoCodeEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

#if 0
//bool AlgoCodeEdit::maybeSave()
//{
//    if (document()->isModified()) { // 如果文档被更改过
//        QMessageBox box;
//        box.setWindowTitle(tr("文档编辑器"));
//        box.setText(tr("是否保存对“%1”的更改？")
//                        .arg(userFriendlyCurrentFile()));
//        box.setIcon(QMessageBox::Warning);

//        // 添加按钮，QMessageBox::YesRole可以表明这个按钮的行为
//        QPushButton *yesBtn = box.addButton(tr("是(&Y)"),QMessageBox::YesRole);
//        box.addButton(tr("否(&N)"),QMessageBox::NoRole);
//        QPushButton *cancelBtn = box.addButton(tr("取消"), QMessageBox::RejectRole);
//        box.exec(); // 弹出对话框，让用户选择是否保存修改，或者取消关闭操作
//        if (box.clickedButton() == yesBtn)  // 如果用户选择是，则返回保存操作的结果
//            return save();
//        else if (box.clickedButton() == cancelBtn) // 如果选择取消，则返回false
//            return false;
//        }
//        return true; // 如果文档没有更改过，则直接返回true
//}

//void AlgoCodeEdit::setCurrentFile(const QString &fileName)
//{
//    //取出路径中的.或..
//    curFile = QFileInfo(fileName).canonicalFilePath();
//    isUntitled = false;
//    document()->setModified(false);
//    setWindowModified(false);
//    setWindowTitle(userFriendlyCurrentFile() + "[*]");
//}

//QString AlgoCodeEdit::getCurFile() const
//{
//    return curFile;
//}

//void AlgoCodeEdit::setCurFile(const QString &value)
//{
//    curFile = value;
//}

//bool AlgoCodeEdit::getIsUntitled() const
//{
//    return isUntitled;
//}

//void AlgoCodeEdit::setIsUntitled(bool value)
//{
//    isUntitled = value;
//}
#endif
