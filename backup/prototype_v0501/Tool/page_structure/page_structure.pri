include(./left_side_switch/left_side_switch.pri)

FORMS += \
    $$PWD/real_home_page.ui \
    $$PWD/tab_page.ui

HEADERS += \
    $$PWD/real_home_page.h \
    $$PWD/tab_page.h \
    $$PWD/store.h \
    $$PWD/page_structure_utils.h

SOURCES += \
    $$PWD/real_home_page.cpp \
    $$PWD/tab_page.cpp \
    $$PWD/store.cpp \
    $$PWD/page_structure_utils.cpp
