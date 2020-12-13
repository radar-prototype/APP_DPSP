#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>
#include <QObject>

// This is available in all editors.
/**
* @projectName   prototype_v0823
* @brief         新建自定义label，可以支持选中和修改文本，但是无法更新文本内容 （算法组件leable 重命名 好像没用到）
* @author        Antrn
* @date          2019-09-02
*/
class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget *parent = nullptr);
    explicit ClickableLabel(const QString &text = "", QWidget *parent = nullptr);
    ~ClickableLabel() override;

signals:
    void lostFocus(ClickableLabel *lab, QString text);
    void selectedChange(ClickableLabel *item);

public slots:
    void labelLostFocus(ClickableLabel *item, QString text);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};

#endif // QTCLICKABLELABEL_H
