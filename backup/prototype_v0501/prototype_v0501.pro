#-------------------------------------------------
#
# Project created by QtCreator 2019-07-12T15:13:11
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += printsupport
QT       += network


//引入
include(./Tool/QTPeopertyBrowser/src/qtpropertybrowser.pri)
include(./Tool/page_structure/page_structure.pri)
include(./RadarPrototype/component/component.pri)
include(./RadarPrototype/DeployManagement/deploymanagement.pri)
include(./RadarPrototype/development_panel/development_panel.pri)
include(./Main/main_window/main_window.pri)
include(./object_prototype/object_prototype.pri)
#---add manually
INCLUDEPATH += $$PWD/AlgoLib
INCLUDEPATH += $$PWD/CalcMachine
INCLUDEPATH += $$PWD/EcmPrototype
INCLUDEPATH += $$PWD/Main
INCLUDEPATH += $$PWD/ObjectPrototype
INCLUDEPATH += $$PWD/RadarPrototype
INCLUDEPATH += $$PWD/Res
INCLUDEPATH += $$PWD/Simulation
INCLUDEPATH += $$PWD/Tool
INCLUDEPATH += $$PWD/RadarPrototype/DeployManagement
#---

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = prototype_v0501
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#win32:LIBS += -lOpengl32 \
#                -lglu32 \
#                -lglut \
#                -lclient
#unix: LIBS += -lclient

SOURCES += \
        Main/main.cpp \
        Main/mainwindow.cpp \
    Main/hellodialog.cpp \
    Main/mysplashscreen.cpp \
    Main/mainwindownew.cpp \
    Main/draglistwidget.cpp \
    Main/mainwindownewscene.cpp \
    RadarPrototype/radaritem.cpp \
    Tool/arrowitem.cpp \
    RadarPrototype/mainwindow_radar.cpp \
    RadarPrototype/radarproperty.cpp \
    RadarPrototype/radarscene.cpp \
    RadarPrototype/diagramitem.cpp \
    RadarPrototype/ppiwidget.cpp \
    RadarPrototype/ppidialog.cpp \
    Tool/arrow.cpp \
    RadarPrototype/diagramtextitem.cpp \
    Tool/compproperty.cpp \
    EcmPrototype/ecmdraglistwidget.cpp \
    EcmPrototype/ecmitem.cpp \
    Tool/clickablelabel.cpp \
    Tool/realtimecurve.cpp \
    Tool/qudong.cpp \
    EcmPrototype/ecmproperty.cpp \
    Tool/runcompconf.cpp \
    Tool/instructionexec.cpp \
    EcmPrototype/mainwindow_ecm.cpp \
    Tool/graphicsScenepublic.cpp \
    Tool/diagramitempublic.cpp \
    RadarPrototype/radarcompdraglistwidget.cpp \
    Tool/utils.cpp \
    Main/menu_iteamoperation.cpp \
    Main/rightwindow_property.cpp \
    Tool/algorithmcomp.cpp \
    Tool/parameditradardialog.cpp \
    RadarPrototype/ashowdialog.cpp \
    RadarPrototype/a_show.cpp \
    Main/projectlist.cpp \
    Tool/echoeswave.cpp \
    Tool/qcustomplot.cpp \
    RadarPrototype/leftnavi.cpp \
    Tool/algocodeedit.cpp \
    Tool/mysyntaxhighlighter.cpp \
    Tool/codewindow.cpp \
    Tool/SimDataGen.cpp \
    Tool/simdatagenwidget.cpp \
    Tool/itemport.cpp\
    RadarPrototype/mainwindow_radar_property.cpp \
    Tool/dataplaybackwidget.cpp \
    Tool/simupparamsetting.cpp \
    Main/mygraphicsview.cpp \
    Tool/codepreview.cpp \
    Tool/sysmessage.cpp \
    Main/platform_entrance.cpp \
    RadarPrototype/arrowproperty.cpp \
    Tool/comp_cpu.cpp \
    Tool/comp_thread.cpp \
    Tool/generatedata.cpp

HEADERS += \
        Main/mainwindow.h \
    Main/hellodialog.h \
    Main/mysplashscreen.h \
    Main/mainwindownew.h \
    Main/draglistwidget.h \
    Main/mainwindownewscene.h \
    RadarPrototype/radaritem.h \
    Tool/arrowitem.h \
    RadarPrototype/mainwindow_radar.h \
    RadarPrototype/radarproperty.h \
    RadarPrototype/radarscene.h \
    RadarPrototype/diagramitem.h \
    RadarPrototype/ppiwidget.h \
    RadarPrototype/ppidialog.h \
    RadarPrototype/leftnavi.h \
    Tool/arrow.h \
    RadarPrototype/diagramtextitem.h \
    Tool/compproperty.h \
    EcmPrototype/ecmdraglistwidget.h \
    EcmPrototype/ecmitem.h \
    Tool/clickablelabel.h \
    Tool/realtimecurve.h \
    Tool/qudong.h \
    EcmPrototype/ecmproperty.h \
    Tool/runcompconf.h \
    Tool/instructionexec.h \
    EcmPrototype/mainwindow_ecm.h \
    Tool/graphicsscenepublic.h \
    Tool/diagramitempublic.h \
    RadarPrototype/radarcompdraglistwidget.h \
    Tool/utils.h \
    Main/menu_iteamoperation.h \
    Main/rightwindow_property.h \
    Tool/algorithmcomp.h \
    Tool/parameditradardialog.h \
    RadarPrototype/ashowdialog.h \
    RadarPrototype/a_show.h \
    Main/projectlist.h \
    Tool/echoeswave.h \
    Tool/qcustomplot.h \
    Tool/algocodeedit.h \
    Tool/linenumberarea.h \
    Tool/mysyntaxhighlighter.h \
    Tool/codewindow.h \
    Tool/SimDataGen.h \
    Tool/simdatagenwidget.h \
    Tool/itemport.h\
    RadarPrototype/mainwindow_radar_property.h \
    Tool/dataplaybackwidget.h \
    Tool/simupparamsetting.h \
    Main/mygraphicsview.h \
    Tool/codepreview.h \
    Tool/sysmessage.h \
    Main/platform_entrance.h \
    RadarPrototype/arrowproperty.h \
    Tool/comp_cpu.h \
    Tool/comp_thread.h \
    Tool/generatedata.h \
    RadarPrototype/requestMsg.h

FORMS += \
    Main/mainwindow.ui \
    Main/hellodialog.ui \
    Main/mainwindownew.ui \
    RadarPrototype/mainwindow_radar.ui \
    RadarPrototype/radarproperty.ui \
    RadarPrototype/ppiwidget.ui \
    RadarPrototype/ppidialog.ui \
    Tool/compproperty.ui \
    Tool/realtimecurve.ui \
    EcmPrototype/ecmproperty.ui \
    Tool/runcompconf.ui \
    EcmPrototype/mainwindow_ecm.ui \
    Main/menu_iteamoperation.ui \
    Main/rightwindow_property.ui \
    Tool/parameditradardialog.ui \
    RadarPrototype/ashowdialog.ui \
    RadarPrototype/a_show.ui \
    Main/projectlist.ui \
    Tool/echoeswave.ui \
    Tool/codewindow.ui \
    Tool/simdatagenwidget.ui \
    Tool/dataplaybackwidget.ui \
    Tool/simupparamsetting.ui \
    Tool/codepreview.ui \
    Main/platform_entrance.ui \
    RadarPrototype/arrowproperty.ui \
    Tool/comp_cpu.ui \
    Tool/comp_thread.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Res/res.qrc

DISTFILES += \
    dds_tcp_conf.ini


unix:!macx: LIBS += -L$$PWD/../../../../../lib/ -lACE \
                    -L$$PWD/../../../../../lib/ -lclient \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_Dcps \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_Federator \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_InfoRepoLib \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_InfoRepoServ \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_InfoRepoDiscovery \
                    -L$$PWD/../../../../../lib/ -lOpenDDS_Tcp \
                    -L$$PWD/../../../../../lib/ -lpublisher \
                    -L$$PWD/../../../../../lib/ -lsubscriber \
                    -L$$PWD/../../../../../lib/ -lTAO \
                    -L$$PWD/../../../../../lib/ -lTAO_AnyTypeCode \
                    -L$$PWD/../../../../../lib/ -lTAO_BiDirGIOP \
                    -L$$PWD/../../../../../lib/ -lTAO_CodecFactory \
                    -L$$PWD/../../../../../lib/ -lTAO_IORTable \
                    -L$$PWD/../../../../../lib/ -lTAO_PI \
                    -L$$PWD/../../../../../lib/ -lTAO_Svc_Utils

# Antrn: 这个不用打开，静态库，已经编译
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lleveldb
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lleveldb
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lleveldb

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lsnappy
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lsnappy
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lsnappy

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lxml2
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lxml2
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lxml2

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lTAO_PortableServer
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lTAO_PortableServer
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lTAO_PortableServer


# Antrn: windows平台先注释
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lcrypto
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lcrypto
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lcrypto

# Antrn: windows平台先注释
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lprotobuf
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lprotobuf
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lprotobuf

# Antrn: windows平台先注释
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../lib/release/ -lssl
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../lib/debug/ -lssl
#else:unix: LIBS += -L$$PWD/../../../../../lib/ -lssl

unix:!macx: LIBS += -L$$PWD/../../../../../lib/ -lgflags

INCLUDEPATH += $$PWD/../../../../../lib
DEPENDPATH += $$PWD/../../../../../lib

