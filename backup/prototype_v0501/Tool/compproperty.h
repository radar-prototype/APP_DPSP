#ifndef COMPPROPERTY_H
#define COMPPROPERTY_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class CompProperty;
}
/**
 * @brief The CompProperty class 导入和导出组件的路径配置窗口
 */
class CompProperty : public QDialog
{
    Q_OBJECT

public:
    explicit CompProperty(QString name, QWidget *parent = nullptr);
    ~CompProperty();

    QString getUid() const;
    void setUid(const QString &value);

    QString getEquipId() const;
    void setEquipId(const QString &value);

    QGraphicsScene *getScene() const;
    void setScene(QGraphicsScene *value);

    QString getPath() const;
    void setPath(const QString &path);


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::CompProperty *ui;
    // name 是INPUT还是OUTPUT
    QString iofName;
    // 场景
    QGraphicsScene *scene;
    // 选择的数据文件的路径
    QString path_;
    // 对应组件的uuid
    QString uid;
    // 是哪个设备
    QString equipId;
};

#endif // COMPPROPERTY_H
