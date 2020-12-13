#ifndef TAB_PAGE_H
#define TAB_PAGE_H

#include <QWidget>

namespace Ui {
class Tab_page;
}

class Tab_page : public QWidget
{
    Q_OBJECT

public:
    explicit Tab_page(QWidget *parent = 0);
    ~Tab_page();

private:
    Ui::Tab_page *ui;

public slots:
    bool add_page(QWidget *page,QString name);
    bool remove_page(int index);
    bool swith_page(QWidget *page);
    //-1:不存在，其他数字：存在，返回当前的index
    int index_of(QWidget *page);
    bool rename_page(QWidget *page,QString new_name);
signals:
    void tabCloseRequested(QWidget *page,bool* if_successful);
};

#endif // TAB_PAGE_H
