#ifndef COMPONENT_INFO_DIALOG_H
#define COMPONENT_INFO_DIALOG_H
#include "algorithmcomp.h"
#include <QDialog>
#include <QAbstractButton>
#include <QListWidgetItem>
#include "qtpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
namespace Ui
{
class Component_info_dialog;
}

class Component_info_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Component_info_dialog(QWidget *parent = nullptr);
    ~Component_info_dialog();
    void add_dialog(AlgorithmComp* ac);
    void edit_dialog(AlgorithmComp* ac);
    void set_right_info(bool is_new_comp, QString name, bool is_property, bool isReadonly);
    void set_info_page(bool is_new_comp);
    void set_left_list();

    int getFlag() const;
    void setFlag(int value);

signals:
    void succeed(QString name_component);
    void edit_succeed(AlgorithmComp* ac);
private slots:
    void on_pushButton_save_clicked();
    void on_pushButton_delete_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_pushButton_p_s_clicked();
    void on_pushButton_p_d_clicked();
    void on_pushButton_Path_clicked();

private:
    //标识符号：0：默认 组件库中使用，1：参数设置中使用。
    int flag=0;
    bool if_new_component=false;
    Ui::Component_info_dialog *ui;
    //公共的 manager
    QtStringPropertyManager *stringManager;
    QtIntPropertyManager *intManager;
    QtBoolPropertyManager *boolManager;
    QtEnumPropertyManager *enumManager;
    QtGroupPropertyManager *groupManager;

    //公共的 factory 关系是否可以编辑
    QtCheckBoxFactory *checkBoxFactory;
    QtLineEditFactory *lineEditFactory ;
    QtEnumEditorFactory *comboBoxFactory;
    QtSpinBoxFactory *spinBoxFactory;

//    算法组件
    AlgorithmComp ac;
    QMap<QString, QMap<QString, QString>> temp_property;

    QtProperty *item0_property;
    QtProperty *item1_property;
    QtProperty *item2_property;
    QtProperty *item3_property;
    QtProperty *item4_property;
    QtProperty *item5_property;
    QtProperty *item0_port;
    QtProperty *item1_port;
    QtProperty *item2_port;
    QtProperty *item3_port;
    QtProperty *item4_port;
    QtProperty *item5_port;

    void pushButton_save(bool if_property);
    void save_info_page();

    void update_right(QtProperty* item, int val);
    void  update_right_item5(QtProperty* item, int val);
    void display_right(QWidget *w_right, bool isReadonly, bool is_property);

};

#endif // RADAR_INFO_DIALOG_H
