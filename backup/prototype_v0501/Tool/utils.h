#ifndef UTILS_H
#define UTILS_H
#include "algorithmcomp.h"
#include <QGraphicsView>
#include <QRect>
#include <QString>
#include <QWidget>
#include <mainwindow_radar.h>
#include <radarcompdraglistwidget.h>
#include <radarscene.h>
#include <QSet>
#include <QtNetwork/QHostAddress>
// This is available in all editors.
/**
* @projectName   prototype_v0906
* @brief         简介
* @author        Antrn
* @date          2019-09-27
*/
class Utils
{
public:
    Utils();

    // 绿色小窗口提示
    static bool alert(QRect rect, QString content);
    // 路径新建（支持多级）
    static QDir openDirOrCreate(QString dirPath);
    // 复制目录，支持循环遍历
    static bool qCopyDirectory(const QDir &fromDir, const QDir& toDir, bool bCoverIfFileExists);
    //shao：删除的文件夹或文件
    static bool deleteFileOrFolder(const QString &strPath);
    //shao：拷贝文件
    static bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    // Antrn: 复制某个文件到指定位置
    static bool qCopyFile(const QString& fromFile, const QString& toFile);
    //shao：拷贝文件夹
    static bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);
    //shao: 获取文件夹下的文件名
    static QStringList getDirFileList(QString file_path);
    //shao:修改文件名称
    static bool modifyFileName( QString filePath, QString fileName, QString newName);

    // 保存算法组件设计面板的场景
    static int saveFile(QWidget *qw, QString dirp, QString filename, RadarScene *scene, bool isPrompt, QString &spath);
    // 不用啦（保存快照  场景图片）
    static int saveImage(int f, RadarScene *scene,  QGraphicsView *view,  QString path, QString name);
    // XML--->算法组件
    static AlgorithmComp readPluginXmlFile(QString fileName);
    // 算法组件--->XML
    static void writeAlgorithmComp2Xml(AlgorithmComp ac, QString subPath = nullptr);
    // 修改文件名（组件库中用 写死了  待修改）
    static bool modifyFileName(QString fileName, QString newName);
    // 通过名字删除XML文件
    static bool deleteXmlFileByName(QString name, QString id);
    // 生成Icon（根据名字）
    static bool generateIcon(QString name, QString eQuip_id);
    // 新建雷达文件夹
    static bool createProject(QString p_name);
    // 总体仿真的XML
    static bool readProjectXml(QString project_path, QMap<QString, QString> &infomap, QMap<QString, QString> &compmap);
    static bool writeProjectXml(QString pat, QString name, QMap<QString, QString> infoMaps, QMap<QString, QString> compMaps);
    static void importXml(QListWidget *listWidget, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem *> *id_item, int flag, QString name);
    // 总体仿真添加设备组件（雷达，目标，对抗）
    static void add_listItem(QListWidget *listWidget, QString name, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem *> *id_item, int flag, QString n);
    static void addItem2List(QString path, QString name, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem*> *id_item, QListWidget *listWidget, QString n);
    // hello页面的
    static QMap<QString, QString> readProjectList();
    // hello页面的项目列表的叉叉点击后触发
    static bool delete1Project(QString pname);
    // 添加一个（新建一个工程后保存）
    static bool addProject2Pl(QString name, QString ppath);
    // 代码生成
    static bool codeGenerate(QList<DiagramItem *> *dlist, MainWindow_Radar *qMain, QString radar_code_file);
    // 删除代码文件
    static void deleteCppFileByName(QString path, QString cppName);
    // 删除整个代码目录
    static void deleteCodeDirByName(QString path, QString codeName);
    // 获取服务器节点数量
    static int getServerNums();
    // 获取服务器IP列表
    static QList<QString> getServerList();
    // 分发代码
    static bool despatchCode(QString server_text_path);
    // 为DDS生成配置
    static bool generateXml4DDS(QString fp, QString fn, QString ip, QString port, int algoCnt, QList<QString> currAlgoIds, QMap<QString, QList<QString>> nextAlgoIds);
    // ..RPC...........
    static bool generateXml4RPC(QString fp, QString fn, QList<QString> serIp, QList<QString> fromPath,
                                QList<QString> toPath, QList<QString> shellPkg);
    // 修改输入输出组件XML路径
    static bool modifyCompXmlPath(QString xmlPath, QString filePath, QString equipId, QGraphicsScene *sce);
    // 自动监测输入输出文件目录的位置
    static QString getOutputOrInputDirName(QString dirname, QString inorOut);
    // 解析qss文件过程封装
    static QString ReadQssFile(const QString& filePath);
    // 实现字符串转换为首字母大写
    static QString toCapital(const QString& string);
    // Antrn: 生成generate.c文件模板
    static bool generateDataCodeFile(QList<DiagramItem*> dl, QString radar_id);
    // Antrn: get this node's IP address
    static QHostAddress getThisNodeIP();
    // Antrn: 进度条
    static void showProgressBar();
    // Antrn: 生成Main.c
    static void generateMain(QList<DiagramItem *> *dlist, QString maincPath);
    static bool startDCPS();
};
#endif // UTILS_H
