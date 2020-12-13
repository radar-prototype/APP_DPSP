#ifndef CODEWINDOW_H
#define CODEWINDOW_H

#include <QFile>
#include <QGraphicsItem>
#include <QListWidgetItem>
#include <QMainWindow>
#include <diagramitem.h>

namespace Ui
{
class CodeWindow;
}
/**
 * @brief The CodeWindow class 组件右击 编辑算法代码窗口
 */
class CodeWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit CodeWindow(QWidget *parent = nullptr, QListWidgetItem *item = nullptr, QGraphicsItem *gitem = nullptr);
    CodeWindow(DiagramItem *gitem);
    bool eventFilter(QObject *,QEvent *event)override;
    // flag=1代表正常的组件右键查看代码 flag=0代表是拖到场景的时候自动生成的代码
    void newFile(const QString &fileName, DiagramItem *dg, int flag);
    bool loadFile(const QString &fileName);
    bool save();

    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();

    QString getCurFile() const;
    void setCurFile(const QString &value);
    bool getIsUntitled() const;
    void setIsUntitled(bool value);

    // 自动填充代码模板
    void autoFillCodeTemplate(QFile &file_cpp, QFile &file_h, QFile &file_t, QString fileName, DiagramItem *dg, QList<QMap<QString, QString>>params);
    void generateCodeByTemplate(QFile &file_cpp, QFile &file_h, QFile &file_t, QString fileName, DiagramItem *dg);

    bool save(QString fn,QString content);
    //shao:新的算法组件代码输入框生成
    QString component_code_input(QList<QString> gitemCodeList, DiagramItem *gitem);
    //shao:主函数代码 （主要是整体查看 微小修改）
    void main_code_disply(QString radar_code_file);

public slots:

    void documentWasModified(); // .cpp文档被更该市，窗口显示更改状态标志
    void document_hWasModified(); // .h文档面板被修改
    void document_tWasModified(); // txt初始化文件

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_actionRedo_triggered();

    void on_actionUndo_triggered();

    void on_actionFull_triggered();

    void on_tabWidget_tabBarClicked(int index);

    void on_actionRegenerate_triggered();

signals:
    void regenerate();

private:
    Ui::CodeWindow *ui;
    bool maybeSave(); //是否需要保存
    QString curFile; //当前文件路径
    bool isUntitled; //当前文件是否未被保存
};

#endif // CODEWINDOW_H
