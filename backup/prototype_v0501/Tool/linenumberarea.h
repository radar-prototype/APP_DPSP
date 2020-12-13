#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include "algocodeedit.h"
/**
 * @brief The LineNumberArea class 代码编辑行号
 */
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(AlgoCodeEdit *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    //返回合适的窗体大小
    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    //重写绘制函数，调用CodeEditor的方法
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    AlgoCodeEdit *codeEditor;
};

#endif // LINENUMBERAREA_H
