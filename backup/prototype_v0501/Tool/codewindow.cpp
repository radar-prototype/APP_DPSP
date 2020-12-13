#include "codewindow.h"
#include "ui_codewindow.h"
#include "utils.h"
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QPushButton>
#include <QListWidgetItem>
#include <QMessageBox>
#include <diagramitem.h>
#include <QTextCodec>

// This is an auto-generated comment.
/**
* @projectName   prototype_v0501
* @brief         代码编辑窗口，包括高亮、行号、注释，支持键盘组合键Ctrl+S,Ctrl+C,Ctrl+V,Ctrl+Z...
* @author        Antrn
* @date          2019-12-30
*/

/**
 * @brief CodeWindow::CodeWindow 构造函数，初始化ui，安装事件过滤器
 * @param parent 父对象
 * @param litem 传入的左边的算法组件列表的对象
 * @param gitem 传入的场景中的算法组件列表的对象
 */
CodeWindow::CodeWindow(QWidget *parent, QListWidgetItem *litem, QGraphicsItem *gitem) :
    QMainWindow(parent),
    ui(new Ui::CodeWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    // Antrn: 分别初始化设置3个tab的属性
    ui->plainTextEdit->installEventFilter(this);
    // 设置tab距离
    ui->plainTextEdit->setTabStopWidth(40);
    ui->plainTextEdit->setMinimumSize(800, 600);

    ui->plainTextEdit_2->installEventFilter(this);
    // 设置tab距离4
    ui->plainTextEdit_2->setTabStopWidth(40);
    ui->plainTextEdit_2->setMinimumSize(800, 600);

    ui->plainTextEdit_3->installEventFilter(this);
    // 设置tab距离4
    ui->plainTextEdit_3->setTabStopWidth(40);
    ui->plainTextEdit_3->setMinimumSize(800, 600);

    // 初始isUntitled为true，未被重命名
    setIsUntitled(false);
    // 保存name
    QString na;

    if( litem == nullptr && gitem == nullptr)
    {
        //shao:无参数的构造 会带这里来
        qDebug() << "你不应该看见这几个字，表示item都为null";//shao:秀
        return;
    }
    else if(litem != nullptr) // 组件库中的组件右键编辑代码
        // shao：还没有作为一个雷达的算法组件的时候就有代码了？？ 那这部分代码不就应该和组件库中的组件一一对应，为啥要这部分代码呢？不懂，所以没有修改）
        // FIXME 这部分暂时有bug，先不能展示组件库中的代码了
    {
        na = litem->text();
        setWindowTitle(na);
//        QString id = litem->data(Qt::UserRole+2).toString();
//        qDebug() << "编辑组件库组件的代码: " << na;
        QString code_path_name = QDir::currentPath() + "/codes/" + na.toLower();
        qDebug() << "编辑组件库组件的代码：" << code_path_name;
        if(!loadFile(code_path_name))
        {
            // 表示读取代码文件失败，需要新建文件
            newFile(code_path_name, nullptr, 1);
        }
        ui->plainTextEdit->showNormal();
//        child->move((QApplication::desktop()->width() - child->width())/2,(QApplication::desktop()->height() - child->height())/2);
    }
    else   //这里仅仅获取代码文件的名称
    {
        DiagramItem *ditem =  qgraphicsitem_cast<DiagramItem *>(gitem);
        na = ditem->getIconName();
        setWindowTitle(na);
//        QString code_path_name = QDir::currentPath() + "/radar/" + ditem->getRadar_id() + "/room/codes/" + na + ditem->itemSuuid.mid(1, ditem->itemSuuid.size() - 2) + "/";
//        Utils::openDirOrCreate(code_path_name);
//        QString code_fname = code_path_name + na.toLower();
//        qDebug() << "编辑场景中组件的代码：" << code_fname;
//        if(!loadFile(code_fname))
//        {
//            // 表示读取代码文件失败，需要新建文件
//            newFile(code_fname, ditem, 1);
//        }
//        ui->plainTextEdit->showNormal();
    }
    //设置tab标签名
    ui->tabWidget->setTabText(0, na.toLower() + ".cpp");
    ui->tabWidget->setTabText(1, na.toLower() + ".h");
    ui->tabWidget->setTabText(2, na.toLower() + ".txt");
}

CodeWindow::CodeWindow(DiagramItem *gitem): ui(new Ui::CodeWindow)
{
    ui->setupUi(this);
    QString code_dir_name = QDir::currentPath() + "/radar/" + gitem->getRadar_id() + "/room/codes/" + gitem->getIconName() + gitem->itemSuuid.mid(1, gitem->itemSuuid.size() - 2) + "/";
    Utils::openDirOrCreate(code_dir_name);
    QString code_fname = code_dir_name + gitem->getIconName().toLower();
    newFile(code_fname, gitem, 0);
}

/**
 * @brief CodeWindow::eventFilter
 * @param widget 指明操作在哪个控件
 * @param event 捕捉到的事件，有事件存在，先在这里捕获进行处理
 * @return bool类型，自己处理完成与否
 */
bool CodeWindow::eventFilter(QObject *widget, QEvent *event)
{
    if(widget == ui->plainTextEdit || widget == ui->plainTextEdit_2 || widget == ui->plainTextEdit_3)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(event);
            // Ctrl+S保存操作
            if (ev->key() == Qt::Key_S && (ev->modifiers() & Qt::ControlModifier))
            {
                if(ui->plainTextEdit->document()->isModified() || ui->plainTextEdit_2->document()->isModified() || ui->plainTextEdit_3->document()->isModified())
                {
                    save();
                    // return true 说明处理完毕，false就是得继续下一步进行处理
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * 新建3种代码文件
 * @brief CodeWindow::newFile
 * @param fileName 不带后缀
 */
void CodeWindow::newFile(const QString &fileName, DiagramItem *dg, int flag)
{
    qDebug() << "新建组件" << fileName << "的.cpp,txt和.h代码文件";
    QString cppName = fileName + ".cpp", hName = fileName + ".h", tName = fileName + ".txt";
    QFile file(cppName), file_h(hName), file_t(tName);
    file.open(QIODevice::WriteOnly);
    file_h.open(QIODevice::WriteOnly);
    file_t.open(QIODevice::WriteOnly);
    // 写入模板内容
    QList<QMap<QString, QString>> params;
    // flag=1代表正常的组件右键查看代码 flag=0代表是拖到场景的时候自动生成的代码
    if(flag == 1)
    {
        autoFillCodeTemplate(file, file_h, file_t, fileName, dg, params);
    }
    else
    {
        generateCodeByTemplate(file, file_h, file_t, fileName, dg);
    }
}
/**
 * @brief CodeWindow::documentWasModified 槽函数，检测文档对象是否被修改
 */
void CodeWindow::documentWasModified()
{
    //设置文档的属性isModified
    setWindowModified(ui->plainTextEdit->document()->isModified());
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setIsUntitled(true);
}

void CodeWindow::document_hWasModified()
{
    setWindowModified(ui->plainTextEdit_2->document()->isModified());
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setIsUntitled(true);
}

void CodeWindow::document_tWasModified()
{
    setWindowModified(ui->plainTextEdit_3->document()->isModified());
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setIsUntitled(true);
}

/**
 * @brief CodeWindow::loadFile 加载代码文件
 * @param fileName 文件路径名，不带后缀
 * @return 布尔类型，代表是否成功
 */
bool CodeWindow::loadFile(const QString &fileName)
{
    QString cppName = fileName + ".cpp";
    QString hName = fileName + ".h";
    QString tName = fileName + ".txt";
    //新建QFile对象
    QFile file(cppName);
    QFile file_h(hName);
    QFile file_t(tName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text) || !file_h.open(QIODevice::ReadOnly | QIODevice::Text) || !file_t.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // 在主窗口中间显示
//        int x =this->x() + (this->width() - aboutdialog.width()) / 2;
//        int y =this->y() + (this->height() - aboutdialog.height()) / 2;
        // 没有必要显示提示，多此一举
//        QMessageBox::warning(this, tr("代码编辑器"),tr("无法读取文件%1:\n%2.").arg(fileName).arg(file.errorString()));
        qDebug() << "[TIP]无法自动打开代码文件，可能是不存在或代开出错";
        return false;
    }
    QTextStream in(&file), in_h(&file_h), in_t(&file_t);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    in.setCodec(QTextCodec::codecForName("utf-8"));
    in_h.setCodec(QTextCodec::codecForName("utf-8"));
    in_t.setCodec(QTextCodec::codecForName("utf-8"));

    ui->plainTextEdit->setPlainText(in.readAll());
    ui->plainTextEdit_2->setPlainText(in_h.readAll());
    ui->plainTextEdit_3->setPlainText(in_t.readAll());

    QApplication::restoreOverrideCursor();
    file.close();
    file_h.close();
    file_t.close();

    setCurFile(fileName);
    // 加载完文件之后，文件内容是认为未修改
    connect(ui->plainTextEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    connect(ui->plainTextEdit_2->document(), SIGNAL(contentsChanged()), this, SLOT(document_hWasModified()));
    connect(ui->plainTextEdit_3->document(), SIGNAL(contentsChanged()), this, SLOT(document_tWasModified()));
    return true;
}

// 保存
bool CodeWindow::save()
{
    return saveFile(getCurFile());
}

/**
 * 截取文件名出来
 * @brief CodeWindow::userFriendlyCurrentFile
 * @return
 */
QString CodeWindow::userFriendlyCurrentFile()
{
    //从文见路径中提取文件名
    return QFileInfo(curFile).baseName();
}

QString CodeWindow::getCurFile() const
{
    return this->curFile;
}

bool CodeWindow::getIsUntitled() const
{
    return isUntitled;
}

void CodeWindow::setIsUntitled(bool value)
{
    isUntitled = value;
}
/**
 * @author Antrn
 * @date 2020-06-10
 * @brief CodeWindow::autoFillCodeTemplate 填入提前写好的代码模板内容，
 *        先读取模板的内容然后通过字符串处理替换掉固定的内容，然后写入到文件，填入两个tab代码编辑框
 * @param file_cpp .cpp代码的对象地址
 * @param file_h .h代码的对象地址
 * @param fileName 代码对应的类（算法）的名字，不带后缀
 * @param params 代码中可能用到的参数
 */
void CodeWindow::autoFillCodeTemplate(QFile &file_cpp, QFile &file_h, QFile &file_t, QString fileName, DiagramItem *dg,
                                      QList<QMap<QString, QString> > params)
{
    // Antrn: 截去前面的路径
    QString nameOnly = fileName.split("/").last();
    // Antrn: 新建文件流
    QTextStream out_cpp(&file_cpp), out_h(&file_h), out_t(&file_t);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // FIXME 这三个文件还不支持自动生成
    QFile file_temp_h(QDir::currentPath() + "/codes/template_h.txt");
    QFile file_temp_cpp(QDir::currentPath() + "/codes/template_cpp.txt");
    // Antrn: 第二种方案，实现代码生成，后来舍弃 DEPRECATED
//    QFile file_temp_cpp(QDir::currentPath() + "/codes/compTemplateCpp.txt");
    QFile file_temp_t(QDir::currentPath() + "/codes/template_Init.txt");
    if ((!file_temp_h.open(QIODevice::ReadOnly | QIODevice::Text)) ||
            (!file_temp_cpp.open(QIODevice::ReadOnly | QIODevice::Text)) ||
            (!file_temp_t.open(QIODevice::ReadOnly | QIODevice::Text)))
    {
        qDebug() << "三种模板文件之一打开失败";
        return;
    }
    // Antrn: 设置文件读取编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString allString_h = codec->toUnicode(file_temp_h.readAll());
    QString allString_cpp = codec->toUnicode(file_temp_cpp.readAll());
    QString allString_t = codec->toUnicode(file_temp_t.readAll());

    QString allProperties = "";
    QString msgLen = "20";
    QString dataSize = "32767";
    // 替换掉固定的字符串
    allString_cpp.replace("[DPCOMPNAMEALLCAPTICAL]", dg->callFunction).replace("_DATETIME_", QDate::currentDate().toString() + " " + QTime::currentTime().toString())
    .replace("_AUTHOR_", "Antrn").replace("_FUNCTION_", "这里填写功能简介").replace("_NAME_", dg->CN_Name)
    .replace("_VERSION_", "1.0").replace("_CREATEDATE_", QDate::currentDate().toString()).replace("[DPCOMPNAMECAPTICAL]", Utils::toCapital(nameOnly)).replace("[THISAREAISPROPERTIESINITIAL]", allProperties).replace("[DPCOMPNAMELOWERCASE]", nameOnly.toLower()).replace("[DPCOMPIP]", dg->getIn_whichIP()).replace("[MSGQUELEN]", msgLen).replace("[DATASIZE]", dataSize);

    allString_h.replace("_DATETIME_", QDate::currentDate().toString() + " " + QTime::currentTime().toString())
    .replace("_AUTHOR_", "Antrn").replace("_FUNCTION_", "这里填写功能简介").replace("_NAME_", dg->CN_Name)
    .replace("_VERSION_", "1.0").replace("_CREATEDATE_", QDate::currentDate().toString()).replace("[DPCOMPNAMEALLCAPTICAL]", dg->callFunction).replace("[DPCOMPNAMEUPPERCASE]", nameOnly.toUpper()).replace("[DPCOMPNAMELOWERCASE]", nameOnly.toLower()).replace("THREADNUM", QString::number(dg->getNum_thread())).replace("[DPCOMPNAMECAPTICAL]", Utils::toCapital(nameOnly)).replace("[DPCOMPNAMEALLUPPERCASE]", dg->callFunction.toUpper());

    // 写入文件
    out_cpp << allString_cpp;
    out_h << allString_h;
    out_t << allString_t;

    // 填入编辑框
    ui->plainTextEdit->setPlainText(allString_cpp);
    ui->plainTextEdit_2->setPlainText(allString_h);
    ui->plainTextEdit_3->setPlainText(allString_t);

    QApplication::restoreOverrideCursor();

    // 文件内容已经保存
    setCurFile(fileName);

    file_temp_h.close();
    file_temp_cpp.close();
    file_temp_t.close();

    file_cpp.close();
    file_h.close();
    file_t.close();
}

void CodeWindow::generateCodeByTemplate(QFile &file_cpp, QFile &file_h, QFile &file_t, QString fileName, DiagramItem *dg)
{
    // Antrn: 截去前面的路径
    QString nameOnly = fileName.split("/").last();
    // Antrn: 新建文件流
    QTextStream out_cpp(&file_cpp), out_h(&file_h), out_t(&file_t);

    // FIXME 这三个文件还不支持自动生成
    QFile file_temp_h(QDir::currentPath() + "/codes/template_h.txt");
    QFile file_temp_cpp(QDir::currentPath() + "/codes/template_cpp.txt");
    QFile file_temp_t(QDir::currentPath() + "/codes/template_Init.txt");
    if ((!file_temp_h.open(QIODevice::ReadOnly | QIODevice::Text)) ||
            (!file_temp_cpp.open(QIODevice::ReadOnly | QIODevice::Text)) ||
            (!file_temp_t.open(QIODevice::ReadOnly | QIODevice::Text)))
    {
        qDebug() << "三种模板文件之一打开失败";
        return;
    }
    // Antrn: 设置文件读取编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString allString_h = codec->toUnicode(file_temp_h.readAll());
    QString allString_cpp = codec->toUnicode(file_temp_cpp.readAll());
    QString allString_t = codec->toUnicode(file_temp_t.readAll());

    QString allProperties = "";
    QString msgLen = "20";
    QString dataSize = "32767";
    // 替换掉固定的字符串
    allString_cpp.replace("[DPCOMPNAMEALLCAPTICAL]", dg->callFunction).replace("_DATETIME_", QDate::currentDate().toString() + " " + QTime::currentTime().toString())
    .replace("_AUTHOR_", "Antrn").replace("_FUNCTION_", "这里填写功能简介").replace("_NAME_", dg->CN_Name)
    .replace("_VERSION_", "1.0").replace("_CREATEDATE_", QDate::currentDate().toString()).replace("[DPCOMPNAMECAPTICAL]", Utils::toCapital(nameOnly)).replace("[THISAREAISPROPERTIESINITIAL]", allProperties).replace("[DPCOMPNAMELOWERCASE]", nameOnly.toLower()).replace("[DPCOMPIP]", dg->getIn_whichIP()).replace("[MSGQUELEN]", msgLen).replace("[DATASIZE]", dataSize);

    allString_h.replace("_DATETIME_", QDate::currentDate().toString() + " " + QTime::currentTime().toString())
    .replace("_AUTHOR_", "Antrn").replace("_FUNCTION_", "这里填写功能简介").replace("_NAME_", dg->CN_Name)
    .replace("_VERSION_", "1.0").replace("_CREATEDATE_", QDate::currentDate().toString()).replace("[DPCOMPNAMEALLCAPTICAL]", dg->callFunction).replace("[DPCOMPNAMEUPPERCASE]", nameOnly.toUpper()).replace("[DPCOMPNAMELOWERCASE]", nameOnly.toLower()).replace("THREADNUM", QString::number(dg->getNum_thread())).replace("[DPCOMPNAMECAPTICAL]", Utils::toCapital(nameOnly)).replace("[DPCOMPNAMEALLUPPERCASE]", dg->callFunction.toUpper());

    // 写入文件
    out_cpp << allString_cpp;
    out_h << allString_h;
    out_t << allString_t;

    // 文件内容已经保存
    setCurFile(fileName);

    file_temp_h.close();
    file_temp_cpp.close();
    file_temp_t.close();

    file_cpp.close();
    file_h.close();
    file_t.close();
}
/**
 * 自定义路径另存为
 * @brief CodeWindow::saveAs
 * @return
 */
bool CodeWindow::saveAs()
{
    //FIXME:需要针对不同的框修改的
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), getCurFile());
    //获取文件路径，为空返回false，否则保存
    if(fileName.isEmpty())
    {
        qDebug() << "另存为路径为空！";
        return false;
    }
    else
    {
        return saveFile(fileName);
    }
}
//shao:传入一个文件路径 带后缀的 更有用 能复用
bool CodeWindow::save(QString fn, QString content)
{
    QFile file(fn);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text) )
    {
        QMessageBox::warning(this, tr("多代码编辑器"), tr("无法写入文件 %1:\n%2.").arg(fn).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("utf-8"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << content;

    QApplication::restoreOverrideCursor();
    setCurFile(fn.split('.')[0]);
    // 文件内容已经保存
    file.close();
    return true;
}
/**
 * @brief CodeWindow::saveFile
 * @param fileName 绝对路径+文件名
 * @return 是否保存成功
 */
bool CodeWindow::saveFile(const QString &fileName)
{
    QString fn = fileName;
    QString name_cpp = fn + ".cpp", name_h = fn + ".h", name_t = fn + ".txt";

    this->save(name_cpp, ui->plainTextEdit->toPlainText());
    this->save(name_h, ui->plainTextEdit_2->toPlainText());
    this->save(name_t, ui->plainTextEdit_3->toPlainText());

    setWindowTitle(userFriendlyCurrentFile());
    setIsUntitled(false);
    return true;
}
/**
 * 设置当前的文件名，设置已经保存
 * @brief CodeWindow::setCurFile
 * @param fileName
 */
void CodeWindow::setCurFile(const QString &fileName)
{
    this->curFile =  fileName;
}
/**
 * 弹窗提醒未保存
 * @brief CodeWindow::maybeSave
 * @return
 */
bool CodeWindow::maybeSave()
{
    //FIXME:保存之后还需要再选保存 不对劲
    // 先检查是否经过修改
//    qDebug() << ui->plainTextEdit->document()->isModified() << ui->plainTextEdit_2->document()->isModified() << ui->plainTextEdit_3->document()->isModified();
    if (getIsUntitled())   // 如果代码被更改过
    {
        QMessageBox box;
        box.setWindowTitle(tr("代码编辑器"));
        box.setText(tr("是否保存对\" %1 \"的更改？")
                    .arg(userFriendlyCurrentFile()));
        box.setIcon(QMessageBox::Warning);

        // 添加按钮，QMessageBox::YesRole可以表明这个按钮的行为
        QPushButton *yesBtn = box.addButton(tr("是(&Y)"), QMessageBox::YesRole);
        box.addButton(tr("否(&N)"), QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("取消"), QMessageBox::RejectRole);
        box.exec(); // 弹出对话框，让用户选择是否保存修改，或者取消关闭操作
        if (box.clickedButton() == yesBtn)  // 如果用户选择是，则返回保存操作的结果
            return save();
        else if (box.clickedButton() == cancelBtn) // 如果选择取消，则返回false
            return false;
    }
    return true; // 如果代码没有更改过，则直接返回true
}
/**
 * 关闭窗口事件
 * @brief CodeWindow::closeEvent
 * @param event
 */
void CodeWindow::closeEvent(QCloseEvent *event)
{
    // 关闭前检查，如果返回true，则关闭窗口
    if (maybeSave())
    {
        event->accept();
    }
    else       // 否则忽略该事件
    {
        event->ignore();
    }
}
// 保存按钮
void CodeWindow::on_actionSave_triggered()
{
    this->save();
}

void CodeWindow::on_actionSaveAs_triggered()
{
    this->saveAs();
}

void CodeWindow::on_actionRedo_triggered()
{
    if(ui->plainTextEdit->isVisible())
    {
        qDebug() << "当前是cpp窗口";
        ui->plainTextEdit->redo();
    }
    else if(ui->plainTextEdit_2->isVisible())
    {
        qDebug() << "当前是h窗口";
        ui->plainTextEdit_2->redo();
    }
    else
    {
        ui->plainTextEdit_3->redo();
    }
}

void CodeWindow::on_actionUndo_triggered()
{
    if(ui->plainTextEdit->isVisible())
    {
        qDebug() << "当前是cpp窗口";
        ui->plainTextEdit->undo();
    }
    else if(ui->plainTextEdit_2->isVisible())
    {
        qDebug() << "当前是h窗口";
        ui->plainTextEdit_2->undo();
    }
    else
    {
        ui->plainTextEdit_3->undo();
    }
}

void CodeWindow::on_actionFull_triggered()
{
    this->showMaximized();
}

void CodeWindow::on_tabWidget_tabBarClicked(int index)
{
    qDebug() << "当前打开第" << index << "个tab";
}

//shao：算法组件初始代码生成，显示，之后让用户输入（点击右击按钮触发）
QString CodeWindow::component_code_input(QList<QString> codeList, DiagramItem *gitem)
{
    //存代码的目录
    QString code_dir_name = QDir::currentPath() + "/radar/" + gitem->getRadar_id() + "/room/codes/" + gitem->getIconName() + gitem->itemSuuid.mid(1, gitem->itemSuuid.size() - 2) + "/";
    Utils::openDirOrCreate(code_dir_name);
    QString fileName = code_dir_name + gitem->getIconName().toLower();
    //    qDebug() << "新建组件" << fileName << "的.cpp,txt和.h代码文件";
    QString cppName = fileName + ".cpp", hName = fileName + ".h", tName = fileName + ".txt";
    QFileInfo file(cppName);
    if(!file.exists())
    {
        qDebug() << "代码模板写入";
        // 文件不存在，新建，修改；
        QFile file_cpp(cppName), file_h(hName), file_t(tName);
        file_cpp.open(QIODevice::WriteOnly);
        file_h.open(QIODevice::WriteOnly);
        file_t.open(QIODevice::WriteOnly);
        // Antrn: 截去前面的路径
        QString nameOnly = fileName.split("/").last();
        // Antrn: 新建文件流
        QTextStream out_cpp(&file_cpp), out_h(&file_h), out_t(&file_t);
        // shao: 定义编码方式  写入
        out_h.setCodec(QTextCodec::codecForName("utf-8"));
        out_h << codeList[0] << endl;
        out_cpp.setCodec(QTextCodec::codecForName("utf-8"));
        out_cpp << codeList[1] << endl;
        out_t.setCodec(QTextCodec::codecForName("utf-8"));
        out_t << codeList[2] << endl;
        file_cpp.close();
        file_h.close();
        file_t.close();
    }
    this->setCurFile(fileName);//存当前代码文件路径
    this->loadFile(fileName);//文件存在就直接显示了
    return "1";
}

void CodeWindow::main_code_disply(QString radar_code_file)
{

    QFile file(radar_code_file);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "[TIP]main_code_disply无法自动打开代码文件，可能是不存在或代开出错";
    }
    QTextStream in(&file);
    in.setCodec(QTextCodec::codecForName("utf-8"));
    this->ui->plainTextEdit->setPlainText(in.readAll());
    file.close();
    QFileInfo info(radar_code_file);
    this->setCurFile(radar_code_file.split(".")[0]);//存当前代码文件路径

    delete this->ui->plainTextEdit_2;
    delete this->ui->plainTextEdit_3;
    //FIXME: 要不要显示默认代码 这样所有代码文件都能看到了 Antrn： 啥意思啊？能说明白不
}


void CodeWindow::on_actionRegenerate_triggered()
{
    emit this->regenerate();
}
