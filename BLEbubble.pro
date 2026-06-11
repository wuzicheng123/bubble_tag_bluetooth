#-------------------------------------------------
#
# Project created by QtCreator 2026-05-18T16:43:09
#
#-------------------------------------------------

QT       += core gui widgets bluetooth axcontainer printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = BLEbubble
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    msgbox.cpp \
    qcustomplot.cpp \
    mycustomplot.cpp \
    wordengine.cpp

HEADERS += \
    Define.h \
        mainwindow.h \
    msgbox.h \
    qcustomplot.h \
    mycustomplot.h \
    wordengine.h

FORMS += \
        mainwindow.ui

include(3rdparty/xlsx/qtxlsx.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_PROJECT_DEPTH = 0

RESOURCES += \
    resources.qrc

RC_ICONS = ./icons/f2pv3-g40d7-001.ico