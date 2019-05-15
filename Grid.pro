#-------------------------------------------------
#
# Project created by QtCreator 2018-10-29T22:05:26
#
#-------------------------------------------------

QT       += datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Grid
TEMPLATE = app


SOURCES += main.cpp\
    data.cpp \
        mainwindow.cpp \
    scene.cpp \
    surfacegraph.cpp

HEADERS  += mainwindow.h \
    scene.h \
    data.h \
    surfacegraph.h

RESOURCES += \
    resources.qrc
