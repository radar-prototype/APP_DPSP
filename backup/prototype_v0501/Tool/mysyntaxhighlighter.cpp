#include "mysyntaxhighlighter.h"

#include <QRegularExpression>

MySyntaxHighlighter::MySyntaxHighlighter(QTextDocument *parent):
    QSyntaxHighlighter (parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);//设定关键词的高亮样式
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns; //关键词集合,关键都以正则表达式表示 下面的可以改为我们想要的关键词
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b" << "\\bnullptr\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b" << "\\bNULL\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b" << "\\bendif\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b" << "\\bif 0\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b" << "\\bdefine\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b" << "\\binclude\\b" << "\\bifndef\\b"
                    << "\\bstring\\b" << "\\breturn\\b" << "\\bnullptr\\b" << "\\busing\\b" << "\\bdelete\\b";

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);//添加Qt的类到高亮规则中
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);//单行注释
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(QColor(0, 128, 0)); //多行注释，只设定了样式，具体匹配在highlightBlock中设置

    quotationFormat.setForeground(Qt::darkGreen);//字符串
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    headFileFormat.setForeground(QColor(0, 128, 0)); //头文件
    rule.pattern = QRegularExpression("<.*>");
    rule.format = headFileFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);//函数
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression("/\\*");//多行注释的匹配正则表达式
    commentEndExpression = QRegularExpression("\\*/");
}

void MySyntaxHighlighter::highlightBlock(const QString &text)
{
//    QTextCharFormat textCharfmt;
//    //高亮色：blue
//    textCharfmt.setForeground(Qt::blue);
//    textCharfmt.setFontWeight(75);
//    //正则表达式
//    QString pattern = "\\bchar\\bint\\bclass\\b";
//    QRegExp expression(pattern);
//    //从0开始匹配字符串
//    int index = text.indexOf(expression);
//    //如果匹配成功，返回字符串的其实位置，大于等于0
//    while(index >= 0){
//        int length = expression.matchedLength();
//        setFormat(index, length, textCharfmt);
//        index = text.indexOf(expression, index + length);
//    }
    foreach (const HighlightingRule &rule, highlightingRules)  //文本采用高亮规则
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0); //以下是多行注释的匹配
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
