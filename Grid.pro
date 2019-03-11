#-------------------------------------------------
#
# Project created by QtCreator 2018-10-29T22:05:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Grid
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    scene.cpp \

HEADERS  += mainwindow.h \
    scene.h \
    random.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
