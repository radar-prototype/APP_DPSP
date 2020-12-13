#ifndef CODEPREVIEW_H
#define CODEPREVIEW_H

#include <QWidget>

namespace Ui {
class CodePreview;
}
/**
 * @brief The CodePreview class 代码预览（代码生成 后的预览  和代码生成类似  但是不能合起来）
 */
class CodePreview : public QWidget
{
    Q_OBJECT

public:
    explicit CodePreview(QString fn, QWidget *parent = nullptr);
    bool eventFilter(QObject *,QEvent *event)override;
    ~CodePreview();
    bool save();
    bool loadFile(QString fpath);
    QString userFriendlyCurrentFile();

    QString getCodeFileName() const;
    void setCodeFileName(const QString &value);

public slots:
    void documentWasModified();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::CodePreview *ui;
    QString codeFileName;
    bool maybeSave();
};

#endif // CODEPREVIEW_H
