#ifndef ALGOCODEEDIT_H
#define ALGOCODEEDIT_H

#include "mysyntaxhighlighter.h"

#include <QObject>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QToolBar>
/**
 * @brief The AlgoCodeEdit class 组件右击 编辑算法代码窗口内部的代码编辑框（是一个大的控件）
 */
class AlgoCodeEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit AlgoCodeEdit(QWidget*parent = nullptr);
//    void newFile(const QString &fileName);
//    bool loadFile(const QString &fileName);
//    QString userFriendlyCurrentFile();
//    QString currentFile(){
//        return curFile;
//    }
//    void setCurrentFile(const QString &fileName);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

//    void keyPressEvent(QKeyEvent *event) override;

//    bool getIsUntitled() const;
//    void setIsUntitled(bool value);

//    QString getCurFile() const;
//    void setCurFile(const QString &value);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
//    void documentWasModified(); //文档被更改时，窗口显示更改状态标志
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
//    bool maybeSave(); //是否需要保存
//    QString curFile; //当前文件路径
//    bool isUntitled; //当前文件是否未被保存

    QWidget *lineNumberArea;
    MySyntaxHighlighter *mshlighter;
    const QFont *font;
};

#endif // ALGOCODEEDIT_H
