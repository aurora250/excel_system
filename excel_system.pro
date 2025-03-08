QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    arguwindow.cpp \
    funcwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    mathlib.cpp

HEADERS += \
    arguwindow.h \
    caculate.h \
    caculate_table_item.h \
    funcwindow.h \
    graph.h \
    mainwindow.h \
    mathlib.h \
    setting.h

FORMS += \
    arguwindow.ui \
    funcwindow.ui \
    mainwindow.ui

TRANSLATIONS += \
    excel_system_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

QXLSX_PARENTPATH=$$PWD/QXlsx/
QXLSX_HEADERPATH=$$PWD/QXlsx/header/
QXLSX_SOURCEPATH=$$PWD/QXlsx/source/
include($$PWD/QXlsx/QXlsx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
