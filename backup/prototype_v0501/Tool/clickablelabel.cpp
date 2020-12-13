#include "clickablelabel.h"
#include <QDebug>
#include <QTextCursor>

ClickableLabel::ClickableLabel(QWidget *parent)
{

}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent):QLabel (parent)
{
    this->setText(text);
}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug() << "双击";
    if (textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QLabel::mouseDoubleClickEvent(event);
}

void ClickableLabel::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this, text());
    qDebug() << text();
    QLabel::focusOutEvent(event);
}

void ClickableLabel::labelLostFocus(ClickableLabel *item, QString text)
{
    qDebug() << "失去光标, text: " + text;
    item->setText(text);
}
