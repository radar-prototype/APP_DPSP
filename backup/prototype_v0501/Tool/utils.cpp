#include "utils.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QLabel>
#include <QPainter>
#include <QRect>
#include <QTextStream>
#include <QTimer>
#include <radarscene.h>
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QMessageBox>
/**
* @projectName   prototype_v0906
* @brief         简介:工具类
* @author        Antrn
* @date          2019-09-27
*/
Utils::Utils()
{
    //构造函数
}

/**
* @projectName   prototype_v0906
* @brief         简介: 弹窗工具
* @author        Antrn
* @date          2019-09-27
*/
bool Utils::alert(QRect rect, QString content)
{
    QLabel *label = new QLabel;
    // 5.6.1
    label->setWindowFlags(Qt::FramelessWindowHint);
    // 5.9.8
    // label->setWindowFlag(Qt::FramelessWindowHint);
    label->setStyleSheet("background-color:rgb(212,237,218)");
    label->setMaximumWidth(400);
    label->setMaximumHeight(50);
    label->setMinimumWidth(400);
    label->setMinimumHeight(50);

    label->setText(content);
    label->setAlignment(Qt::AlignCenter);
    label->setGeometry(int((rect.width() - label->width()) / 2), int((rect.height() - label->height()) / 2), label->width(), label->height());
//    label->setGeometry(int((rect.width()-label->width())/2), int((rect.height()-label->height())/2), label->width(), label->height());
    label->show();
    QTimer::singleShot(1300, label, [ = ]()
    {
        label->close();
        delete label;
        return true;
    });
    // Antrn: 防止野指针
    label = nullptr;
    return true;
}


/**
 * @brief Utils::saveFile 简介: 保存文件到某个文件夹的文件中
 * @param qw 父类组件
 * @param dirp 要打开的文件目录
 * @param filename 文件名
 * @param scene 场景对象
 * @param isPrompt 是否弹窗询问
 * @param spath
 * @return 返回结果表明：
 * @author Antrn
 * @date 2019-09-27
 */
int Utils::saveFile(QWidget *qw, QString dirp, QString filename, RadarScene *scene, bool isPrompt, QString &spath)
{
    QString directory = dirp;
    openDirOrCreate(directory);
    if(isPrompt)
    {
        spath = QFileDialog::getExistingDirectory(
                    qw,
                    "根据当前工程名选择要保存入的文件夹",
                    directory,
                    QFileDialog::ShowDirsOnly);
    }
    // 用户选择了其他位置时执行
    if (!spath.isEmpty() && directory != spath + "/")
    {
        QFile file(spath + "/" + filename); // 这个斜杠很关键
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.close();
        }
        else
        {
            QTextStream out(&file);
            scene->getDoc()->save(out, 4); //EncodingFromDocument
            file.close();
            qDebug() << "场景保存成功!路径为：" + spath;
        }
    }
    // 保存到默认文件夹，必执行
    if (!directory.isEmpty())
    {
        QFile file(directory + "/" + filename); // 这个斜杠很关键
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.close();
            return -1;
        }
        else
        {
            QTextStream out(&file);
            scene->getDoc()->save(out, 4); //EncodingFromDocument
            file.close();
            qDebug() << "雷达场景保存路径为：" + directory;
            return 1;
        }
    }
    else
    {
        qDebug() << "请选择存储位置后再保存!";
        return 0;
    }
}


/**
 * @brief Utils::saveImage 保存快照截图
 * @param f flag：场景还是视图
 * @param scene 场景指针
 * @param view 视图指针
 * @param path 保存路径
 * @param name 文件名字
 * @return 整型flag
 * @author Antrn
 * @date 2019-09-27
 */
int Utils::saveImage(int f, RadarScene *scene, QGraphicsView *view,  QString path, QString name)
{
    int width = 0, height = 0;
    // 场景
    if(f == 1)
    {
        width = static_cast<int>(scene->width());
        height = static_cast<int>(scene->height());
    }
    else
    {
        width = static_cast<int>(view->width());
        height = static_cast<int>(view->height());
    }
    QSize mysize(width, height);
    QImage image(mysize, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    if(f == 1)
    {
        scene->render(&painter);//也可以使用视图保存，只保存视图窗口的快照
    }
    else
    {
        view->render(&painter);
    }
    painter.end();
    QString dir_str = path;
    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(dir_str))
    {
        dir.mkpath(dir_str);
    }
    image.save(dir_str + name);
    return 0;
}

/**
 * @brief 没有就创建目录
 * @param dirPath
 * @return 成功与否
 */
QDir Utils::openDirOrCreate(QString dirPath)
{
    qDebug() << "即将检测是否创建目录" << dirPath;
    QDir dir(dirPath);
    if(!dir.exists())
    {
        qDebug() << "创建的目录是否成功" << dir.mkpath(dirPath);//创建多级目录
    }
    else
        qDebug() << "已经存在，无需创建";
    return dir;
}

/**
* @projectName   prototype_v0906
* @brief         简介 读取算法组件的xml文件到Algorithm
* @author        Antrn
* @date          2019-10-02
*/
AlgorithmComp Utils::readPluginXmlFile(QString fileName)
{
    AlgorithmComp ac;
    QDomDocument doc;
    if(!fileName.isEmpty())
    {
//        qDebug() << fileName;
        // 中文时会读取失败，所以转化为utf-8
        QFile file(fileName.toUtf8());
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "算法组件" << fileName << "的xml文件打开出错！";
            file.close();
            return ac;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            qDebug() << "算法组件的xml文件读取失败, 请查看是否是中文命名";
            return ac;
        }
        file.close();
        // 根元素component
        QDomElement docElem = doc.documentElement();
        // 第一个孩子是<Information>
//        QDomNode n = docElem.firstChild();
        QDomNode itemNode = doc.elementsByTagName("Information").at(0);
        // 子孩子就是标签名为Name、ID...
        QDomNode m = itemNode.firstChild();
        QMap<QString, QString> infomap;
        while(!m.isNull())
        {
            std::string tagName = m.nodeName().toStdString();
            if(m.isElement())
            {
                // 每个元素item
                QString content = m.toElement().text();
                // 保存起来
//                qDebug() << QString::fromStdString(tagName) << ": " << content;
                infomap.insert(QString::fromStdString(tagName), content);
            }
            m = m.nextSibling();
        }
        ac.setInfo(infomap);
        // Description
        QDomNode descNode = doc.elementsByTagName("Description").at(0);
        QString desc = descNode.toElement().text();
//        qDebug() << "desc: " << desc;
        ac.setDesc(desc);

        // 大的标签是Parameter的时候
        QDomNode ParaNode = doc.elementsByTagName("Parameter").at(0);
        // 就是arrow了，因为箭头就一种
        QDomNode m1 = ParaNode.firstChild();
        QString describe, type, isoutput, value;
        QMap<QString, QMap<QString, QString> >paramap;
        // 遍历所有的属性
        while(!m1.isNull())
        {
            std::string tagName = m1.nodeName().toStdString();
            if(m1.isElement())
            {
                // 每个元素item
                QDomElement e = m1.toElement();
                describe = e.attribute("describe");
                type = e.attribute("type");
                isoutput = e.attribute("isoutput");
                value = e.attribute("value");
//                qDebug() << QString::fromStdString(tagName) << ": describe： " << describe << "; " << "type: " << type;
                QMap<QString, QString> mm;
                mm.insert("describe", describe);
                mm.insert("type", type);
                mm.insert("isoutput", isoutput);
                mm.insert("value", value);
                paramap.insert(QString::fromStdString(tagName), mm);
            }
            m1 = m1.nextSibling();
        }
        ac.setParam(paramap);

//        QFileInfo fi = QFileInfo(fileName);
//        QString file_name = fi.fileName().split(".")[0];  //获取文件名
//        qDebug() << "短名字： " << file_name;
//        ac.setFileName(file_name);
        ac.setFileName(ac.getInfo()["Name"]);
//        qDebug() << ac.getInfo()["ID"];
        return ac;
    }
    else
    {
        // 文件名为空，啥也没选，提示
        Utils::alert(QApplication::desktop()->screen()->rect(), "请选择文件!");
        return ac;
    }
}

/**
* @projectName   prototype_v0906
* @brief         简介 将算法组件存到xml文件中,
* @author        Antrn
* @date          2019-10-03
* @param ac 算法对象
* @param subPath 子目录，代表用户拖动到场景中复制出来的算法组件
*/
void Utils::writeAlgorithmComp2Xml(AlgorithmComp ac, QString subPath)
{
    QString savePath;
    if(subPath.isNull())
    {
        savePath = QDir::currentPath() + "/algoXml/";
    }
    else
    {
        QDir *dir = new QDir;
        dir->mkdir(QDir::currentPath() + subPath);
        savePath = QDir::currentPath() + subPath + "/algoXml/";
    }

    openDirOrCreate(savePath);

    QMap<QString, QString> infoMap = ac.getInfo();
    QMap<QString, QMap<QString, QString>> paraMap = ac.getParam();
    QString desc = ac.getDesc();
    QDomDocument doc;
    QDomElement Info, Desc, Param;
    //添加处理指令即xml头部说明和场景的属性
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("Plugins");
    doc.appendChild(root);
    Info = doc.createElement("Information");
    Desc = doc.createElement("Description");
    root.appendChild(Info);
    root.appendChild(Desc);
    Param = doc.createElement("Parameter");
    root.appendChild(Param);

    // info
    for (QMap<QString, QString>::iterator i = infoMap.begin(); i != infoMap.end(); i++)
    {
        QDomElement info = doc.createElement(i.key());

        QDomText c = doc.createTextNode(i.value());
#if 0
        // 更新时间，不为空说明是复制到工程空间的，需要更改时间和id等信息
        if(!subPath.isNull())
        {
            QString dtime;
            if(!i.key().compare("Time"))
            {
                QDateTime *dt = new QDateTime;
                dtime = dt->currentDateTime().toString();
                c = doc.createTextNode(dtime);
            }
            else
            {
                c = doc.createTextNode(i.value());
            }
        }
#endif
        info.appendChild(c);
        Info.appendChild(info);
    }
    QDomText d = doc.createTextNode(ac.getDesc());
    Desc.appendChild(d);
    // params
    for (QMap<QString, QMap<QString, QString>>::iterator j = paraMap.begin(); j != paraMap.end(); j++)
    {
        QDomElement para = doc.createElement(j.key());
        QDomAttr describe = doc.createAttribute("describe");
        QDomAttr type = doc.createAttribute("type");
        describe.setValue(j.value().value("describe"));
        QDomAttr isoutput = doc.createAttribute("isoutput");
        isoutput.setValue(j.value().value("isoutput"));
        QDomAttr value = doc.createAttribute("value");
        value.setValue(j.value().value("value"));
        // 5.9.8
//        qDebug() << "参数Map：" << j.value().toStdMap();
        type.setValue(j.value().value("type"));

        para.setAttributeNode(describe);
        para.setAttributeNode(type);
        para.setAttributeNode(isoutput);
        para.setAttributeNode(value);
        Param.appendChild(para);
    }
    QString id_ = ac.getInfo().take("ID");

    QString filename;
    if(!subPath.isNull())
    {
        // 当有子目录存在的时候，存在子目录中的算法组件命名为name+id,由于id两边有大括号，所以去除
        filename = savePath + ac.getFileName() + id_.mid(1, id_.length() - 2) + ".xml";
    }
    else
    {
        filename = savePath + ac.getFileName() + ".xml";
    }
    QFile file(filename); // 这个斜杠很关键
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.close();
        qDebug() << "算法XML打开文件失败";
    }
    else
    {
        QTextStream out(&file);
        doc.save(out, 4); //EncodingFromDocument
        file.close();
        qDebug() << "算法组件保存成功!路径为： " + filename;
    }
}

/**
* @projectName   prototype_v0906
* @brief         简介 重命名文件，必须要使用静态方法，且绝对路径，否则失败
* @author        Antrn
* @date          2019-10-04
*/
bool Utils::modifyFileName(QString fileName, QString newName)
{
    QString filePath = QDir::currentPath() + "/algoXml/" + fileName + ".xml";
    qDebug() << "修改文件名前文件完整路径： " << filePath;
    AlgorithmComp ac = readPluginXmlFile(filePath);
    ac.setFileName(newName);
    if(QFile::exists(filePath))
    {
//        bool b = QFile::rename(filePath, QDir::currentPath()+"/algoXml/"+newName+".xml");
//        return b;
        QFile::remove(filePath);
    }
    else
    {
        qDebug() << "源文件不存在!";
//        return false;
    }
    writeAlgorithmComp2Xml(ac);
    return true;
}

/**
 * @brief 动态删除场景里产生的xml 拷贝算法组件空间的文件
 * @param dname 文件夹路径
 * @param id 组件的id
 * @return 是否成功
 */
bool Utils::deleteXmlFileByName(QString dname, QString id)
{
    QDir dir(dname);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for(int i = 0; i < file_list.size(); ++i)
    {
        if(file_list.at(i).fileName().split(".")[0].endsWith(id))
        {
            QFile file(file_list.at(i).absoluteFilePath());
//            qDebug() << "finfo.absoluteFilePath()： " << file_list.at(i).absoluteFilePath();
            if (file.exists())
            {
                qDebug() << "删除文件--->" << file_list.at(i).absoluteFilePath();
                return file.remove();
            }
            else
            {
                qDebug() << "!!文件" << dname << "不存在";
            }
        }
    }
    return false;
}

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-09-30 T 16:21:34
 * @copyright (c)
 * @status DEPRECATED 弃用
 * @brief Utils::generateIcon 将名字写到图片上，并保存到文件夹中
 * @param nm 图片上要写的名字
 * @param eQuip_id 设备名字
 * @return 是否成功
 */

bool Utils::generateIcon(QString nm, QString eQuip_id)
{
    QString p = QDir::currentPath() + "/images/base.png";
    QImage image = QPixmap(p).toImage();
    // 为这个QImage构造一个QPainter
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    // 设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
    painter.setRenderHint(QPainter::TextAntialiasing);
    // 改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    pen.setWidth(20);
    QFont font = painter.font();
    font.setBold(true);//加粗
    font.setWeight(81);
    font.setPixelSize(11);//改变字体大小
    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(image.rect(), Qt::AlignCenter, nm);
    // 将nmame写在Image的中心
    int n = 100;//这个为图片的压缩度。0/100
    // 将画好的图片保存起来。 当为空的时候说明是往算法组件库中添加组件
    qDebug() << "-------[重要名字]---------" << nm;
    if(eQuip_id == "")
    {
        qDebug() << "将" << nm << ".ico存入当前组件库";
        image.save(QDir::currentPath() + "/images/" + nm + ".ico", "ico", n);
    }
    else
    {
        // WARNING 这容易出bug，保存多次。 往小的雷达项目中添加组件
        image.save(QDir::currentPath() + "/radar/" + eQuip_id + "/images/" + nm + ".ico", "ico", n);
    }
    return true;
}

/**
 * @brief 新建工程目录文件
 * @param p_name 工程名
 * @return 成功与否
 */
bool Utils::createProject(QString p_name)
{
    qDebug() << "雷达id：" << p_name;
    QDir d(QDir::currentPath());
    if(d.exists(QDir::currentPath() + "/radar/" + p_name))
    {
        qDebug() << "目录已存在!";
        return true;
    }
//    qDebug() << "测试是否执行";
    QString path1 = QDir::currentPath() + "/radar/" + p_name + "/room/waveData/";
    QString path2 = QDir::currentPath() + "/radar/" + p_name + "/room/algoXml/";
    QDir dir;
    //创建多级目录，如果已存在则会返回去true
    if(dir.mkpath(path1) && dir.mkpath(path2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 读取工程的.dpsp配置文件
 * @param project_path 项目文件路径
 * @param infomap 保存读取的工程信息
 * @param compmap 工程中其他元素的信息
 * @return 是否读取成功
 */
bool Utils::readProjectXml(QString project_path, QMap<QString, QString> &infomap, QMap<QString, QString> &compmap)
{
    QDomDocument doc;
    if(!project_path.isEmpty())
    {
        QFile file(project_path);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "project的dpsp文件打开出错,请查看是否存在！";
            file.close();
            return false;
        }
        if(!doc.setContent(&file))
        {
            file.close();
            qDebug() << "project的dpsp文件读取失败!";
            return false;
        }
        file.close();

        // project-root
        QDomElement docElem = doc.documentElement();
        // 第一个孩子是<information>
        // QDomNode n = docElem.firstChild();
        QDomNode itemNode = doc.elementsByTagName("information").at(0);
        // 子孩子就是标签名为name、id
        QDomNode m = itemNode.firstChild();
        while(!m.isNull())
        {
            std::string tagName = m.nodeName().toStdString();
            if(m.isElement())
            {
                // 每个元素item
                QString content = m.toElement().text();
                // 保存起来
                // qDebug() << QString::fromStdString(tagName) << ": " << content;
                infomap.insert(QString::fromStdString(tagName), content);
            }
            m = m.nextSibling();
        }

        // 大的标签是components的时候,小标签是radar,ecm,object
        QDomNode m1 = doc.elementsByTagName("components").at(0).childNodes().at(0);
        while(!m1.isNull())
        {
            if(m1.isElement())
            {
                QString text = m1.toElement().tagName();
                QString id, name;
                if(text.compare("radar") == 0)
                {
                    id = m1.toElement().attribute("id");
                    name = m1.toElement().attribute("name");
                    qDebug() << "读取到radar： id：" << id << "name:" << name;

                }
                else if(text.compare("ecm") == 0)
                {
                    id = m1.toElement().attribute("id");
                    name = m1.toElement().attribute("name");
                    qDebug() << "读取到ecm: id:" << id << "name:" << name;
                }
                else if(text.compare("object") == 0)
                {
                    id = m1.toElement().attribute("id");
                    name = m1.toElement().attribute("name");
                    qDebug() << "读取到object: id:" << id << "name:" << name;
                }
                // 测试时，当遍历到ecm和object时，id和name都为""
                if(!id.isEmpty() && !name.isEmpty())
                {
                    // 插入后会根据id的英文字母自动排序，所以存储的顺序不是插入顺序
                    compmap.insert(id, name);
                }
            }
            m1 = m1.nextSibling();
        }
        return true;
    }
    else
    {
        // 文件名为空，啥也没选，提示
        Utils::alert(QApplication::desktop()->screen()->rect(), "请选择文件!");
        return false;
    }
}

/**
 * @brief Utils::writeProjectXml
 * @param pat
 * @param name
 * @return
 */
/**
 * @brief Utils::writeProjectXml 保存工程文件到dpsp
 * @param pat dpsp文件的路径
 * @param name dpsp文件名字
 * @param itemMaps 键：radar/ecm/object:键:雷达/对抗/目标 组件的id,值:名字，在这想要扭转雷达以名字作为唯一标识的问题
 * @return 是否写入成功
 */
bool Utils::writeProjectXml(QString pat, QString name, QMap<QString, QString> infoMaps, QMap<QString, QString> compMaps)
{
    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("project-root");
    doc.appendChild(root);

    QDomElement Info = doc.createElement("information");

    QDomElement xname = doc.createElement("name");
    QDomText domname = doc.createTextNode(name);
    xname.appendChild(domname);
    Info.appendChild(xname);
    QDomElement xid = doc.createElement("id");
    // 随机生成project的uuid，不能这样做
    QDomText domid = doc.createTextNode(infoMaps.value("id"));
    xid.appendChild(domid);
    Info.appendChild(xid);

    QDomElement Comps = doc.createElement("components");

    for(auto item_name : compMaps)
    {
        QString iiid = compMaps.key(item_name);
        QString equip_name;
        if(iiid.contains("radar"))
        {
            equip_name = "radar";
        }
        else if(iiid.contains("ecm"))
        {
            equip_name = "ecm";
        }
        else if(iiid.contains("object"))
        {
            equip_name = "object";
        }
        QDomElement xeq = doc.createElement(equip_name);
        QDomAttr xrid = doc.createAttribute("id");
        xrid.setValue(iiid);
        QDomAttr xrname = doc.createAttribute("name");
        xrname.setValue(item_name);
        xeq.setAttributeNode(xrid);
        xeq.setAttributeNode(xrname);
        Comps.appendChild(xeq);
    }
    // 遍历传过来的组件列表的名字
//    for (int i=0; i<itemMaps.size(); i++) {
//        QDomElement xradar = doc.createElement("radar");
//        QDomAttr xrid = doc.createAttribute("id");
//        // 生成雷达的唯一id
//        QString tp = QUuid::createUuid().toString();
//        // 使radar开头，以便渲染图标
//        QString radar_id = tp.mid(1,tp.length()-2);
//        xrid.setValue(equipType+radar_id);
//        QDomAttr xrname = doc.createAttribute("name");
//        xrname.setValue(itemNames[i]);
//        xradar.setAttributeNode(xrid);
//        xradar.setAttributeNode(xrname);
//        Comps.appendChild(xradar);
//    }

//    QDomElement xecm = doc.createElement("ecm");
//    QDomElement xtarget = doc.createElement("object");
//    Comps.appendChild(xecm);
//    Comps.appendChild(xtarget);

    root.appendChild(Info);
    root.appendChild(Comps);

    // 【千万要注意】，改动doc之后要手动保存，我在这上面绊了一脚
    QFile file(pat);
    // 每次打开文件都会先删除后重写
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
//        file.close();
        qDebug() << "dpsp打开文件失败";
        return false;
    }
    else
    {
        QTextStream out(&file);
        doc.save(out, 4); //EncodingFromDocument
        file.close();
        return true;
    }
}

/**
 * @brief Utils::importXml 导入rad/ecm/targ工程的配置并调用读取
 * @param listWidget
 * @param id_inde
 * @param nameSet
 * @param id_item
 * @param flag 1新建 2导入
 * @param name 新建，不同的draglist调用的时候的表明是雷达还是对抗还是目标，为了公用性
 */
void Utils::importXml(QListWidget *listWidget, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem*> *id_item, int flag, QString name)
{
    // 导入
    if(flag == 2)
    {
        QFileInfo fileinfo;
        QString fileName = QFileDialog::getOpenFileName(nullptr, "打开rad/ecm/targ工程", QString(QDir::currentPath()), "files (*.rad *.ecm *.targ)");
        qDebug() << "长路径名字：" << fileName;

        if(fileName.compare("") && !fileName.isEmpty())
        {
            // 先判断导入的是哪种对象
            fileinfo = QFileInfo(fileName);
            // 文件后缀
            // qDebug() << fileinfo.suffix();
            // 文件名
            QString n = fileinfo.baseName();
            if(fileinfo.suffix().compare("rad") == 0)
            {
                add_listItem(listWidget, QString("雷达"), id_inde, nameSet, id_item, flag, n);
            }
            else if(fileinfo.suffix().compare("ecm") == 0)
            {
                add_listItem(listWidget, QString("对抗"), id_inde, nameSet, id_item, flag, n);
            }
            else
            {
                add_listItem(listWidget, QString("目标"), id_inde, nameSet, id_item, flag, n);
            }
        }
    }
    // 新建
    if(flag == 1)
    {
        add_listItem(listWidget, name, id_inde, nameSet, id_item, flag, "");
    }
}


/**
 * @brief Utils::add_listItem 分导入和新建两种，新建则需要计数文件夹数量，向后递增新建item；导入则是保留原有的item名字
 * @param listWidget 组件对象指针
 * @param name 区分是雷达/对抗/目标
 * @param id_inde 列表中的id数量
 * @param nameSet 名字集合
 * @param id_item map，名字到list item指针的映射
 * @param flag 旗帜，1新建，2导入
 * @param n 导入之后的名字
 * shao:改了，这方法不要了
 */
void Utils::add_listItem(QListWidget *listWidget, QString name, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem*> *id_item, int flag, QString n)
{
    QString path;
    // FIXME 有必要枚举 待修缮
    qDebug() << "id_inde: " << *id_inde;

    // 不同目标的文件夹
    QString path_;
    if(name.compare("雷达") == 0)
    {
        path = ":/img/radar.png";
        path_ = QDir::currentPath() + "/radar";
    }
    else if(!name.compare("对抗"))
    {
        path = ":/img/ECM.png";
        path_ = QDir::currentPath() + "/ecm";
    }
    else if(!name.compare("目标"))
    {
        path = ":/img/object.png";
        path_ = QDir::currentPath() + "/object";
    }


    // 新建
    if(flag == 1)
    {
        // 这样每新建一次 数量就会翻倍
//        int fileCount=*id_inde;
        int fileCount = 1;
        // 先检查每种对象分别有几个已存在
        QDir dir(path_);
        qDebug() << "各个设备的path_目录:" << path_;
        QFileInfoList fileInfoList = dir.entryInfoList();
        foreach ( QFileInfo fileInfo, fileInfoList )
        {
            if ( fileInfo.fileName() == "." || fileInfo.fileName() == ".." )
                continue;
            else if ( fileInfo.isDir() )
            {
                // 将所有的name读进去
                nameSet->insert(fileInfo.fileName());
//                qDebug() << fileInfo.fileName();
                fileCount++;
//                qDebug() << fileCount;
            }
            else
            {
                continue;
            }
        }
        if(fileCount != 0)
        {
            *id_inde = fileCount;
        }
        addItem2List(path, name, id_inde, nameSet, id_item, listWidget, n);
    }
    else
    {
        // 导入
        addItem2List(path, name, id_inde, nameSet, id_item, listWidget, n);
    }
}



/**
 * @brief Utils::addItem2List 将item新建并加入到listwidget中
 * @param path icon的路径
 * @param name 雷达/对抗/目标其中之一
 * @param id_inde listwidget中的id计数器，传入指针！不然会崩溃
 * @param nameSet 名字集合，不能重复，程序中保证不能重复
 * @param id_item 存放名字和item的map映射，也要指针
 * @param listWidget 传过来界面上的listwidget的指针
 * @param n 名字，如果是导入的话，需要解析导入文件的名字，也就是导入之后列表项的名字
 */
void Utils::addItem2List(QString path, QString name, int *id_inde, QSet<QString> *nameSet, QMap<QString, QListWidgetItem*> *id_item, QListWidget *listWidget, QString n)
{
    QString newName;
    // 导入
    if(!n.isEmpty() && n.compare(""))
    {
        newName = n;
    }
    // 新建item
    else
    {
        // 添加到左边的listwidget
        int curr = *id_inde;
        *id_inde = curr + 1;
        newName = name + QString::number(curr);

        //    while(this->id_item.contains(newName))
        while(nameSet->contains(newName))
        {
            qDebug() << "名字生成重复" << newName;
            // 名称已经存在，换一个
            curr = *id_inde;
            *id_inde = curr + 1;
            newName = name + QString::number(curr);
            qDebug() << "换一个name" << newName;
        }
    }
    if(nameSet->contains(newName))
    {
        qDebug() << "名字已存在, 导入失败, 说明已经导入相同名字的组件。";
        return;
    }
    else
    {
        nameSet->insert(newName);
        QListWidgetItem *item = new QListWidgetItem();
        id_item->insert(newName, item);
        item->setIcon(QIcon(path));
        item->setText(newName);

        //这里的用户角色存储用户数据（和拖入场景有关）
        item->setData(Qt::UserRole, QPixmap(path));
        item->setData(Qt::UserRole + 1, newName);
        item->setData(Qt::UserRole + 2, name);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
        listWidget->addItem(item);
        qDebug() << "additem了, name:" << newName << "nameSet变成:" << *nameSet << "id_item:" << *id_item;
        // 想要在列表中出现组件之后立刻在radar/object/ecm下新建各自的文件夹
        // 这样解决不同project中新建组件的时候产生雷达组件名重复，因为你只是在列表添加了，但是文件夹并没有新建，这样下次遍历文件夹的时候，这个还是重新新建了一次
        QString s;
        if(name.compare("雷达") == 0)
        {
            s = QDir::currentPath() + "/radar/" + newName + "/";
        }
        else if(!name.compare("对抗"))
        {
            s = QDir::currentPath() + "/ecm/" + newName + "/";
        }
        else if(!name.compare("目标"))
        {
            s = QDir::currentPath() + "/object/" + newName + "/";
        }
        Utils::openDirOrCreate(s);

    }
}

/**
 * @brief Utils::readProjectList 开始页面读取项目列表
 * @return 结果map集合
 */
QMap<QString, QString> Utils::readProjectList()
{
    QString proDir = QDir::currentPath() + "/project/";
//    Utils::openDirOrCreate(proDir);
    QString project_path = proDir + "projectlist.pl";
    QDomDocument doc;
    if(!project_path.isEmpty())
    {
        QFile file(project_path);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "project的.pl文件打开出错,请查看是否存在！";
            file.close();
            return QMap<QString, QString>();
        }
        if(!doc.setContent(&file))
        {
            file.close();
            qDebug() << "project的pl文件读取失败!";
            return QMap<QString, QString>();
        }
        file.close();

        // projects
        QDomElement docElem = doc.documentElement();
        // 第一个孩子是<project>
        QDomNode p = docElem.firstChild();
        QString name, path, dtime;
        QMap<QString, QString> resMap;
        while(!p.isNull())
        {
            if(p.isElement())
            {
                // 每个元素item
                QDomElement e = p.toElement();
                name = e.attribute("name");
                path = QDir::currentPath() + e.attribute("path");
                resMap.insert(name, path);
            }
            p = p.nextSibling();
        }
        return resMap;
    }
    else
    {
        // 文件名为空，啥也没选，提示
        Utils::alert(QApplication::desktop()->screen()->rect(), "请选择文件!");
        return QMap<QString, QString>();
    }
}

/**
 * @brief Utils::delete1Project
 * @param pname 项目名字，根据他对pl文件中的项目进行删除
 * @return 是否成功
 */
bool Utils::delete1Project(QString pname)
{
    QString ppath = QDir::currentPath() + "/project/projectlist.pl";
    QFile file(ppath); //相对路径、绝对路径、资源路径都可以
    if(!file.open(QFile::ReadOnly))
        return false;
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        qDebug() << "project的pl文件读取失败!";
        return false;
    }
    file.close();
    // projects
    QDomElement docElem = doc.documentElement();
    // 第一个孩子是<project>
    // 这种方法可以，下面的也可以
//    QDomNode p = docElem.childNodes().at(0);
//    while(!p.isNull()){
//        if(p.isElement()){
//            // 每个元素item
//            if(!p.toElement().attribute("name").compare(pname)){
//                qDebug()<< "找到对应项目名称：" <<p.toElement().attribute("name");
//                docElem.removeChild(p);
//                return true;
//            }
//        }
//        p = p.nextSibling();
//    }

    QDomNodeList list = doc.elementsByTagName("project"); //由标签名定位
    for(int i = 0; i < list.count(); i++)
    {
        QDomElement e = list.at(i).toElement();
        //以属性名定位，类似于hash的方式，warning：这里仅仅删除一个节点，其实可以加个break
        if(e.attribute("name") == pname || !e.attribute("name").compare(pname))
        {
            qDebug() << ".pl文件中找到对应项目名称：" << e.attribute("name");
            docElem.removeChild(list.at(i));
            // 【千万要注意】，改动doc之后要手动保存，我在这上面绊了一脚
            QFile file(ppath);
            // 每次打开文件都会先删除后重写
            if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                file.close();
                qDebug() << "pl删除时打开文件失败";
            }
            else
            {
                QTextStream out(&file);
                doc.save(out, 4); //EncodingFromDocument
                file.close();
            }
            return true;
        }
    }
    return false;
}

/**
 * @brief Utils::addProject2Pl 向projectlist.pl中添加一条project
 * @param name 项目名字
 * @param ppath 项目路径
 * @return 是否成功
 */
bool Utils::addProject2Pl(QString name, QString ppath)
{
    QString fpath = QDir::currentPath() + "/project/projectlist.pl";
    QFile file(fpath); //相对路径、绝对路径、资源路径都可以
    if(!file.open(QFile::ReadOnly))
        return false;
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        qDebug() << "project的pl文件读取失败!";
        return false;
    }
    file.close();

    QDomNode proNode = doc.elementsByTagName("project").at(0);
    while(!proNode.isNull())
    {
        if(proNode.isElement())
        {
            // 每个元素project
            QDomElement e = proNode.toElement();
            QString x_pname = e.attribute("name");
            if(x_pname.compare(name) == 0)
            {
                qDebug() << "找到了已有的project记录：" << x_pname << "不向文件添加记录！";
                return true;
            }
        }
        proNode = proNode.nextSibling();
    }

    // projects
    QDomElement docElem = doc.documentElement();
    QDomElement project = doc.createElement("project");


    QDomAttr pl_name = doc.createAttribute("name");
    QDomAttr pl_path = doc.createAttribute("path");
    QDomAttr pl_dtime = doc.createAttribute("dtime");

    pl_name.setValue(name);
    QString right_path = ppath.right(ppath.length() - QDir::currentPath().length());
    pl_path.setValue(right_path);

    project.setAttributeNode(pl_name);
    project.setAttributeNode(pl_path);
    project.setAttributeNode(pl_dtime);

    docElem.appendChild(project);

    // 【千万要注意】，改动doc之后要手动保存，我在这上面绊了一脚
    QFile file2(fpath);
    // 每次打开文件都会先删除后重写
    if(!file2.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
//        file2.close();
        qDebug() << "pl打开文件失败";
        return false;
    }
    else
    {
        QTextStream out(&file2);
        doc.save(out, 4); //EncodingFromDocument
        file2.close();
        return true;
    }
}


/**
 * @desc  DEPRECATED 后期删除
 * @brief Utils::codeGenerate 整个Main代码文件生成，每次都是重新覆盖文件中的代码
 * @param dlist 算法组件列表，包括输入和输出组件
 * @param radar_code_file 当前雷达的代码文件的保存路径
 * @return bool
 */
bool Utils::codeGenerate(QList<DiagramItem *> *dlist, MainWindow_Radar *main, QString radar_code_file)
{
#if 0
    QString radar_code_file = QDir::currentPath() + "/radar/" + di->getRadar_id() + "/room/" + di->getRadar_id() + ".c";
    QFile openrf(radar_code_file);
    if(!openrf.open(QIODevice::ReadOnly))
    {
        qDebug() << "Open failed, Check what's wrong2.";
        return false;
    }
    QTextStream readts(&openrf);
    // 读取数据
    QString all_code = readts.readAll();
    qDebug() << "全部数据" << all_code;
    openrf.close();
#endif
    QFile writerf(radar_code_file);
    // 流写入文件
    QTextStream tscode(&writerf);
    if(!writerf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed, Check what's wrong1.";
        return false;
    }
    // Antrn: 先写入头文件引入部分
    tscode << main->code_template_start << endl;
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    // Antrn: 提取每个组件中的初始化部分。init.txt
#if 0
    {
        // 遍历组件列表，提取init中的内容
        for(auto *item : *dlist)
        {
            qDebug() << "[代码生成->]当前组件名字是:" << item->iconName;
            QFile readCodeinitf(QDir::currentPath() + "/radar/" + item->getRadar_id() + "/room/codes/" + item->iconName + item->itemSuuid.mid(1, item->itemSuuid.size() - 2) + "/" + item->iconName.toLower() + ".txt");
            if (!readCodeinitf.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug() << "[ERROR] " << item->iconName << "txt文件读取错误";
                return false;
            }
//        QTextStream in(&readCodeinitf);
//        QString init_content = in.readAll();

            QString init_content = codec->toUnicode(readCodeinitf.readAll());
            tscode << init_content << endl;
//        tscode << item->iconName+"();" << endl;
        }
    }
#endif
    QString code_template_mid;
    // 全局变量，消息队列，每个组件有两个，第一个名字是缩写，当前组件处理后的队列，第二个是缩写+Dat
    for(auto *comp : *dlist)
    {
        // 单独处理sim，但是感觉尽量不这么处理
//        if(comp->iconName.compare("SIM"))
//        {
//            code_template_mid += "MSG_Q_ID gMsg" + Utils::toCapital(comp->iconName) + "Q;\n";
//        }
//        else
//        {
        code_template_mid += "MSG_Q_ID gMsg" + Utils::toCapital(comp->iconName) + "Q;\n" +
                             "MSG_Q_ID gMsg" + Utils::toCapital(comp->iconName) + "DatQ;\n";
//        }
    }
    code_template_mid += "\n";
    // 组件参数全局变量，变量名拼接，小写
    for(auto *comp : *dlist)
    {
        code_template_mid += "Component_Param param_" + comp->iconName.toLower() + ";\n";
    }

    tscode << code_template_mid << endl;

    // 函数调用
    QString commit1 = codec->toUnicode("数据接收函数");
    QString commit2 = codec->toUnicode("数据处理线程");

    if(dlist->size() > 1)//判断组件个数大于1
        tscode << main->Simulate_Component.replace("[_AFTER_FIRST_]", Utils::toCapital(dlist->at(1)->getIconName()));

    int thread_nums = 0;
    // Antrn: component thread
    for(auto *item : *dlist)
    {
        if(dlist->indexOf(item) != 0)
        {
            // 读出来模板信息到字符串中，然后将里面的关键词进行替换，分别形成当前算法的数据接收和运算的代码
            tscode << "//" << item->iconName.toLower() << commit1 << "\n";
//        qDebug() << "打印" << item->iconName << "的调用函数：" << item->callFunction;
            QString funC = main->fun_component;
            funC = funC.replace("[_COMPONENT_NAME_]", item->callFunction)
                   .replace("[_GMSGQ_THIS_]", Utils::toCapital(item->iconName))
                   .replace("[_LOWERCASE_THIS_]", item->getIconName().toLower());
            if(dlist->indexOf(item) != 0)
            {
                funC = funC.replace("[_GMSGQ_LAST_]", Utils::toCapital(dlist->at(dlist->indexOf(item) - 1)->iconName)).replace("[_LOWERCASE_LAST_]"
                        , dlist->at(dlist->indexOf(item) - 1)->getIconName().toLower());
            }
            tscode << funC << endl;

            tscode << "//" << item->iconName.toLower() << commit2 << "\n";
//        qDebug() << "首字母大写:" << Utils::toCapital(item->iconName) << endl;
            QString funT = main->fun_thread;
            funT = funT.replace("[_COMPONENT_NAME_]", item->callFunction)
                   .replace("[_GMSGQ_THIS_]", Utils::toCapital(item->iconName))
                   .replace("[_LOWERCASE_THIS_]", item->iconName.toLower());

            if(dlist->indexOf(item) == dlist->size() - 1)
            {
                funT = funT.replace("[_LOWERCASE_NEXT_]", "NULL");
            }
            else
            {
                funT = funT.replace("[_LOWERCASE_NEXT_]", dlist->at(dlist->indexOf(item) + 1)->getIconName().toLower());
            }
            tscode << funT << endl;
        }
        thread_nums += item->getNum_thread();
    }

    tscode << main->main_params_init.replace("_THREAD_ALL_", QString::number(thread_nums));

    // 获取所有组件的线程总数
    QString lower_name;
    QString Queue_length = "20";
    for(auto *comp : *dlist)
    {
        lower_name = comp->getIconName().toLower();
        QString writeCont = "\tpthread_t tidp_" + lower_name + ";\n";
        writeCont +=
            "\tpthread_t tidp_" + lower_name + "_thread[" + QString::number(comp->getNum_thread()) + "];\n"
            "\tparam_"
            + lower_name + ".component_name = \"" + lower_name + "\";\n" +
            "\tparam_" + lower_name + ".component_ip = \"" + comp->getIn_whichIP() + "\";\n"
            "\tmsgQCreate(&gMsg" + Utils::toCapital(comp->getIconName()) + "Q, " + Queue_length + ", sizeof(rspl_thread_param), MSG_Q_FIFO);\n"
            "\tmsgQCreate(&gMsg" + Utils::toCapital(comp->getIconName()) + "DatQ, " + Queue_length + ", sizeof(rspl_thread_param), MSG_Q_FIFO);\n";
        tscode << writeCont << endl;
    }

    // Antrn: thread
    for(auto *comp : *dlist)
    {
        lower_name = comp->getIconName().toLower();
        QString writeCont = "\tpthread_create(&tidp_" + lower_name + ", NULL, " + comp->callFunction + "_Component, NULL);\n"
                            "\tfor (i = 0; i < " + QString::number(comp->getNum_thread() - 1) + "; i++){\n"
                            "\t\tthread_id[i] = i;\n"
                            "\t\tpthread_create(&tidp_" + lower_name + "_thread[i], NULL, " + comp->callFunction + "_thread, NULL);\n\t}\n";
        tscode << writeCont << endl;
    }

    // Antrn: join
    for(auto *c : *dlist)
    {
        lower_name = c->getIconName().toLower();
        QString writeCont = "\tpthread_join(tidp_" + lower_name + ", NULL);\n"
                            "\tfor (i = 0; i < " + QString::number(c->getNum_thread() - 1) + "; i++){\n"
                            "\t\tpthread_join(tidp_" + lower_name + "_thread[i], NULL);\n\t}\n";
        tscode << writeCont << endl;
    }

    tscode << main->code_template_end << endl;
    writerf.close();
    return true;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-20
 * @brief Utils::deleteCppFileByName 单独删除组件空间的代码文件的方法
 * @param path 文件所在的路径
 * @param cppName cpp代码文件名
 */
void Utils::deleteCppFileByName(QString path, QString cppName)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for(int i = 0; i < file_list.size(); ++i)
    {
        QString fName = file_list.at(i).fileName();
        qDebug() << "[Utils::deleteCppFileByName]--文件名" << fName;
        if(fName.compare(cppName) == 0)
        {
            QFile file(file_list.at(i).absoluteFilePath());
            qDebug() << "finfo.absoluteFilePath()： " << file_list.at(i).absoluteFilePath();
            if (file.exists())
            {
                qDebug() << "也要删除代码文件--->" << file_list.at(i).absoluteFilePath();
                file.remove();
                return;
            }
            else
            {
                qDebug() << "代码文件" << path + cppName << "不存在!!!";
            }
        }
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-20
 * @brief Utils::deleteCodeDirByName 删除代码和数据文件
 * @param path 代码文件夹的路径
 * @param codeName 代码/数据文件夹的名字
 */
void Utils::deleteCodeDirByName(QString path, QString codeName)
{
    QDir dir;
    dir.setPath(path + "/codes/" + codeName);
    dir.removeRecursively();
    dir.setPath(path + "/waveData/" + codeName);
    dir.removeRecursively();
}

/**
 * @brief Utils::setCompNums
 * @return 可用状态的服务器数量
 */
int Utils::getServerNums()
{
    // 先和下面的服务器数量对应
    return 1;
}

/**
 * @brief Utils::getServerList
 * @return 获取服务器的列表，里面存ip地址？？或者有端口，后面再商讨
 */
QList<QString> Utils::getServerList()
{
    QList<QString> sl;
    // 先在这写死了
    sl.insert(0, "192.168.1.12");
//    sl.insert(1, "192.168.1.3");
    return sl;
}

/**
 * @brief Utils::despatchCode
 * @return 是否分发成功？
 */
bool Utils::despatchCode(QString server_text_path)
{
    // TODO 调用BRPC开始分发代码，读取此server.txt文件
    qDebug() << "按照此文件进行分发：" << server_text_path;
    return true;
}

/**
 * @brief Utils::generateXml4DDS 给DDS生成xml配置
 * @author Antrn
 * @date 2020-04-04
 * @param fp xml文件的路径
 * @param fn xml文件的名称
 * @param ip xml配置文件中的ip标签值
 * @param port xml中port标签值
 * @param algoCnt 当前ip的服务器要执行的算法的数目
 * @param currAlgoIds 当前服务器上要执行的算法列表
 * @param nextAlgoIds 当前算法执行完毕后的发布者的主题要用的组件的下一个id标识
 * @return 是否生成成功
 */
bool Utils::generateXml4DDS(QString fp, QString fn, QString ip, QString port, int algoCnt, QList<QString> currAlgoIds, QMap<QString, QList<QString> > nextAlgoIds)
{
    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement dds = doc.createElement("DDS");
    doc.appendChild(dds);

    QDomElement dcps_ = doc.createElement("DCPS");
    QDomElement ip_ = doc.createElement("IP");
    QDomElement port_ = doc.createElement("port");
    dds.appendChild(dcps_);
    QDomText ip_value = doc.createTextNode(ip);
    ip_.appendChild(ip_value);
    dcps_.appendChild(ip_);
    QDomText port_value = doc.createTextNode(port);
    port_.appendChild(port_value);
    dcps_.appendChild(port_);

    QDomElement Algos_ = doc.createElement("Algo");
    dds.appendChild(Algos_);
    QDomElement count_ = doc.createElement("count");
    QDomText count_value = doc.createTextNode(QString::number(algoCnt));
    count_.appendChild(count_value);
    Algos_.appendChild(count_);
    qDebug() << currAlgoIds.size();

    for (int i = 0; i < currAlgoIds.size(); i++)
    {
        QDomElement every_algo = doc.createElement("EveAlgo");
        QDomElement algoid_ = doc.createElement("AlgoID");
        QDomText algoid_value = doc.createTextNode(currAlgoIds.at(i));
        algoid_.appendChild(algoid_value);
        every_algo.appendChild(algoid_);
        for (int j = 0; j < nextAlgoIds.value(currAlgoIds.at(i)).size(); j++)
        {
            QDomElement nextalgoid_ = doc.createElement("nextAlgoID");
            QDomText nextalgoid_value = doc.createTextNode(nextAlgoIds.value(currAlgoIds.at(i)).at(j));
            nextalgoid_.appendChild(nextalgoid_value);
            every_algo.appendChild(nextalgoid_);
        }
        Algos_.appendChild(every_algo);
    }

    // 改动doc之后要手动保存
    QFile file(fp + fn);
    // 每次打开文件都会先删除后重写
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.close();
        qDebug() << "DDS打开文件失败";
        return false;
    }
    else
    {
        QTextStream out(&file);
        doc.save(out, 4); //EncodingFromDocument
        file.close();
        return true;
    }
}

/**
 * @brief 给RPC生成需要的xml文件
 * @author Antrn
 * @date 2020-04-04
 * @param fp xml文件路径
 * @param fn xml文件名字
 * @param serIp 服务器的ip地址
 * @param fromPath 从当前服务器上要发送的文件路径
 * @param toPath 发送到其他服务器上的目标文件路径
 * @param algoPkg 算法相关的数据压缩包
 * @param shellPkg 在shell中要执行的指令command
 * @return 是否成功成功
 */
bool Utils::generateXml4RPC(QString fp, QString fn, QList<QString> serIp, QList<QString> fromPath,
                            QList<QString> toPath, QList<QString> shellPkg)
{
    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement rpc = doc.createElement("RPC");
    doc.appendChild(rpc);
    qDebug() << "多少个node：" << serIp.size();
    for (int i = 0; i < serIp.size(); i++)
    {
        QDomElement node = doc.createElement("node");
        QDomElement si = doc.createElement("server-IP");
        QDomElement fromp = doc.createElement("from-path");
        QDomElement top = doc.createElement("to-path");
        QDomElement shellp = doc.createElement("shell-pkg");
        QDomText IP = doc.createTextNode(serIp.at(i));
        si.appendChild(IP);
        node.appendChild(si);
        QDomText f_p = doc.createTextNode(fromPath.at(i));
        fromp.appendChild(f_p);
        node.appendChild(fromp);
        QDomText tp = doc.createTextNode(toPath.at(i));
        top.appendChild(tp);
        node.appendChild(top);
        QDomText sp = doc.createTextNode(shellPkg.at(i));
        shellp.appendChild(sp);
        node.appendChild(shellp);
        rpc.appendChild(node);
    }

    // 改动doc之后要手动保存
    QFile file(fp + fn);
    // 每次打开文件都会先删除后重写
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.close();
        qDebug() << "RPC打开文件失败";
        return false;
    }
    else
    {
        QTextStream out(&file);
        doc.save(out, 4); //EncodingFromDocument
        file.close();
        // Antrn: 拷贝到根目录
        return Utils::copyFileToPath(fp + fn, QDir::currentPath() + "/" + fn, true);
//        return true;
    }
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-06-28
 * @brief Utils::modifyCompXmlPath 修改组件中输入输出组件中的文件位置
 * @param xmlPath xml所在的雷达空间路径
 * @param filePath 想要改变xml中的txt数据文件路径
 * @return
 */
bool Utils::modifyCompXmlPath(QString xmlPath, QString filePath, QString equipId, QGraphicsScene * sce)
{
    AlgorithmComp ac = readPluginXmlFile(xmlPath);
    QMap<QString, QMap<QString, QString>> params = ac.getParam();
    for(auto i : params)
    {
        qDebug() << i << i.value("value");
    }
    params["file"]["value"] = filePath;
    for(auto i : params)
    {
        qDebug() << i << i.value("value");
    }
    ac.setParam(params);

    RadarScene *rscene;
    if((rscene = dynamic_cast<RadarScene*>(sce)))
    {
        rscene->deleteScene_comps(ac.getInfo()["ID"]);
        // 重新插入新的算法，id不变
        rscene->add2Scene_comps(ac.getInfo()["ID"], ac);
        // 重新写入文件
        writeAlgorithmComp2Xml(ac, "/radar/" + equipId + "/room");
        // 刷新一下属性Dock，默认不能编辑
        rscene->emit refreshPropertyDock(ac, true);
    }
    return false;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-07-04
 * @brief Utils::getOutputOrInputDirName 传入INPUT　或者　OUTPUT　找出对应的文件夹名字
 * @param dirname　上级文件夹名字
 * @param inorOut　INPUT　或者　OUTPUT
 * @return
 */
QString Utils::getOutputOrInputDirName(QString dirname, QString inorOut)
{
    QDir dir(dirname);
    if (!dir.exists())
    {
        qDebug() << dirname << "文件夹不存在!";
        return "error";
    }
    //取到所有的文件夹，但是去掉.和..
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    //文件夹优先
    dir.setSorting(QDir::DirsFirst);
    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    if(list.size() < 1 )
    {
        // 没有文件夹
        qDebug() << dirname << "里面啥都没有";
        return "error";
    }
    int i = 0;
    QFileInfo fileInfo;
    bool isFind = false;
    //递归算法的核心部分
    do
    {
        fileInfo = list.at(i);
        //如果是文件夹，找OUTPUT/INPUT开头的文件夹
        bool bisDir = fileInfo.isDir();
        if(bisDir)
        {
            qDebug() << "文件夹路径和名字" << fileInfo.filePath() << ":" << fileInfo.fileName();
            if(fileInfo.fileName().contains(inorOut))
            {
                isFind = true;
                break;
            }
        }
        else
        {
            qDebug() << "你不应该看到这句话" << fileInfo.filePath() << ":" << fileInfo.fileName();
        }
        //end else
        i++;
    }
    while(i < list.size());

    if(isFind)
        return fileInfo.fileName();
    else
        return "error";
}


/**
 * @brief Utils::qCopyDirectory
 * @param fromDir 源目录, 传进来的都是绝对路径
 * @param toDir 目标目录路径
 * @param bCoverIfFileExists  true同名时候覆盖, false同名时终止拷贝,返回false
 * @return true:拷贝成功 false:拷贝失败
 */
bool Utils::qCopyDirectory(const QDir & fromDir, const QDir & toDir, bool bCoverIfFileExists)
{
    QDir fromDir_ = fromDir;
    QDir toDir_  = toDir;
    if(!toDir_.exists())
    {
        if(!toDir_.mkdir(toDir.absolutePath()))
        {
            return false;
        }
    }

    QFileInfoList fileInfoList = fromDir_.entryInfoList();

    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {
            continue;
        }

        // 如果是目录
        if(fileInfo.isDir())
        {
            if(!qCopyDirectory(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName()), bCoverIfFileExists))
            {
                return false;
            }
        }
        else
        {
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName()))
            {
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
    }
    return true;
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-09-15
 * @brief Utils::qCopyFile 复制文件工具方法
 * @param fromFile 源文件路径
 * @param toFile 目标文件路径
 * @return 是否复制成功
 */
bool Utils::qCopyFile(const QString & fromFile, const QString & toFile)
{
    QFile file(toFile); //将要复制到的新文件完整路径
    //判断文件按是否存在，存在则删除
    if(file.exists())
    {
        qDebug() << file.fileName() << "文件已存在，先删除";
        file.remove();
    }
    // 将文件复制到新的文件路径下
    // Antrn: always return true
    return QFile::copy(fromFile, toFile);
}

/**
 * @brief Utils::ReadQssFile
 * @param filePath qss文件路径 eg：":/qss/left_side_switch.qss"
 * @return
 */
QString Utils::ReadQssFile(const QString & filePath)
{
    QString strStyleSheet = "";
    QFile file(filePath);
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        strStyleSheet = QLatin1String(file.readAll());
    }
    return  strStyleSheet;
}

/**
 * @brief Utils::toCapital 将英文QString字符串转化为首字符大写
 * @param string 字符串参数
 * @return 首字母大写的字符串
 */
QString Utils::toCapital(const QString & string)
{
    QString Cap = string.toUpper();
    return QString(Cap.mid(0, 1) + string.toLower().mid(1, string.size() - 1));
}

// This is an auto-generated comment.
/**
 * @author Antrn
 * @date 2020-09-16
 * @brief Utils::generateDataCodeFile 生成generate.c文件，从组件库code/中读取模板，修改后放在本空间的room下
 * @param dl 顺序组件列表
 * @param radar_id 设备名字（雷达名）
 * @return 是否成功
 */
bool Utils::generateDataCodeFile(QList<DiagramItem *> dl, QString radar_id)
{
    int thread_nums = dl.size();
    QString comp_list_order;
    for (DiagramItem* i : dl)
    {
        qDebug() << i->iconName << "---------xxxxxxxxxx";
        comp_list_order += "\"" + i->iconName + "\",";
    }
    QString s = comp_list_order.mid(0, comp_list_order.size() - 1);

//    qDebug() << comp_list_order << "====" << s;
    // 读取generate.c 模板进行替换
    QString file_path = QDir::currentPath() + "/radar/" + radar_id + "/room/generate.c";
    QFile file_c(file_path);
    if (!file_c.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "写入generate.c打开失败";
        return false;
    }
    QTextStream out_c(&file_c);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QFile file_temp_c(QDir::currentPath() + "/codes/generate.c");
    if (!file_temp_c.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "组件库中generate.c打开失败";
        return false;
    }
    // Antrn: 设置文件读取编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString allString_c = codec->toUnicode(file_temp_c.readAll());

    // 替换掉固定的字符串
    allString_c.replace("__THREAD_NUMS__", QString::number(thread_nums)).replace("__COMPS_LIST_BY_ORDER__", s.toLower());
    // 写入文件
    out_c << allString_c;
    QApplication::restoreOverrideCursor();
    file_temp_c.close();
    file_c.close();
    return true;
}

QHostAddress Utils::getThisNodeIP()
{
    foreach(const QHostAddress &hostAddress, QNetworkInterface::allAddresses())
        if(hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address())
            return hostAddress;
    return QHostAddress::LocalHost;
}

// This is an auto-generated comment by Antrn.
/**
 * @author Antrn
 * @date 2020-09-28 T 21:44:44
 * @copyright (c)
 * @brief Utils::showProgressBar 废物代码，暂时
 */
void Utils::showProgressBar()
{
//    QProgressDialog progressDialog(this);
//    progressDialog.setCancelButtonText(tr("&Cancel"));
//    progressDialog.setRange(0, 100);
//    progressDialog.setWindowTitle(tr("生成代码"));
//    progressDialog.setMinimumDuration(5);
//    progressDialog.setValue(0);
//    progressDialog.setLabelText(tr("正在生成代码，进度（0%）..."));
    //    QCoreApplication::processEvents();
}

void Utils::generateMain(QList<DiagramItem *> *dlist, QString maincPath)
{
    QFile writerf(maincPath);
    // 流读取文件
    QTextStream tscode(&writerf);
    if(!writerf.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Open failed, Check what's wrong in generateMain.";
        return;
    }
    QString headStr = "";
    for(auto *d : *dlist)
    {
        headStr += "#include\"" + d->callFunction + ".h\"\n";
    }
    // Antrn: 先写入头文件引入部分
    tscode << headStr << endl;
    tscode << "int main(){\n";
    QString callComps = "";
    // Antrn: 变量命名考虑可能变量名重复的问题
    for(auto *d : *dlist)
    {
        QString vcompname = d->getIconName().toLower();
        callComps = "\t" + toCapital(d->callFunction) + " " + vcompname + ";\n\t" + vcompname + ".init();\n\t" + vcompname + ".run();\n";
        tscode << callComps;
    }
    tscode << "\treturn 0;\n}" << endl;
    writerf.close();
}

bool Utils::startDCPS()
{
    QProcess *p = new QProcess;
    QStringList args;
    args.append("-ORBListenEndpoints");
    QString iiiip = Utils::getThisNodeIP().toString();
    qDebug() << "this IP: " << iiiip;
    QString IP_args = "iiop://" + iiiip + ":12345";
    args.append(IP_args);
    args.append("-ORBDebugLevel");
    args.append("10");

    QString command = "/home/kylin/xidian-723/20200929/DCPSInfoRepo";
    p->startDetached(command, args);
    p->setStandardErrorFile("/home/kylin/桌面/log.txt");
    p->waitForStarted();
    p->waitForFinished();
    p->close();
    return true;
}

bool Utils::deleteFileOrFolder(const QString &strPath)//要删除的文件夹或文件的路径
{
    if (strPath.isEmpty() || !QDir().exists(strPath))//是否传入了空的路径||路径是否存在
        return false;

    QFileInfo FileInfo(strPath);

    if (FileInfo.isFile())//如果是文件
        QFile::remove(strPath);
    else if (FileInfo.isDir())//如果是文件夹
    {
        QDir qDir(strPath);
        qDir.removeRecursively();
    }
    return true;
}

//拷贝文件：；两个路径都要带上名字
bool Utils::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\", "/");
    if (sourceDir == toDir)
    {
        return true;
    }
    if (!QFile::exists(sourceDir))
    {
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist)
    {
        if(coverFileIfExist)
        {
            createfile->remove(toDir);
        }
    }//end if
    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}
//拷贝文件夹：
bool Utils::copyDirectoryFiles(const QString &source, const QString &destination, bool override)
{
    QDir directory(source);
    if (!directory.exists())
    {
        return false;
    }


    QString srcPath = QDir::toNativeSeparators(source);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();
    QString dstPath = QDir::toNativeSeparators(destination);
    if (!dstPath.endsWith(QDir::separator()))
        dstPath += QDir::separator();


    bool error = false;
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i)
    {
        QString fileName = fileNames.at(i);
        QString srcFilePath = srcPath + fileName;
        QString dstFilePath = dstPath + fileName;
        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            if (override)
            {
                QFile df(dstFilePath);
                if(df.exists())
                    df.remove();
//                QFile::setPermissions(dstFilePath, QFile::WriteOwner);
            }

            QFile::copy(srcFilePath, dstFilePath);
        }
        else if (fileInfo.isDir())
        {
            QDir dstDir(dstFilePath);
            dstDir.mkpath(dstFilePath);
            if (!copyDirectoryFiles(srcFilePath, dstFilePath, override))
            {
                error = true;
            }
        }
    }
    return !error;
}
QStringList Utils::getDirFileList(QString file_path)
{
    QDir dir(file_path);
    QStringList file_list;

    QStringList nameFilters;
    if (dir.exists())   //判断目录是否存在
    {
        /*
        说明
            QDir::NoDotAndDotDot
            作用：去除 "."  ".."   它们分别表示为当前目录、上一级目录

            QDir::Files
            作用：显示文件（不是文件夹）

            QDir::Dirs
            作用：显示文件夹

            代码示例：

                file_list = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
                qDebug() << file_list;

        拓展
            当你需要查询特定内容（比如说固定的文件格式、特定的文件名）
            那你可以根据需要加入前缀、后缀信息来辅助查询

            如果你当前的文件夹很多,如：文件夹1、文件夹2、文件夹3...
            那你可以通过加入前缀信息来辅助查询，
            代码示例：

                nameFilters << "文件*";
                file_list = dir.entryList(nameFilters,QDir::NoDotAndDotDot | QDir::Dirs);
                qDebug() << file_list;

            如果你需要查询特定的文件，如图片
            可以加入文件格式作为后缀信息来辅助查询
            代码示例：

                nameFilters << "*.jpg" << "*.png";
                file_list = dir.entryList(nameFilters,QDir::Files | QDir::NoDotAndDotDot);
                qDebug() << file_list;
        */
        file_list = dir.entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot);
//        qDebug() << file_list;
        return file_list;
    }
    else
    {
        file_list.clear();
        qDebug() << "Utils::getDirFileList-->该目录不存在！！！";
        return file_list;
    }
}
//修改文件名字
bool Utils::modifyFileName( QString filePath, QString fileName, QString newName)
{
    if(QFile::exists(filePath+fileName))
    {
        QFile::rename(filePath+fileName,filePath+newName);
    }
    else if(QDir(filePath+fileName).exists())
    {
       QFile::rename(filePath+fileName,filePath+newName);
    }
    else
    {
        QMessageBox::StandardButton rb = QMessageBox::warning(NULL, "源文件不存在！", "请检查文件名是否正确", QMessageBox::Yes);
        qDebug() << "源文件不存在!";
    }
    return true;
}
