#-------------------------------------------------
#
# Project created by QtCreator 2015-04-07T17:58:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = microMouseServer
TEMPLATE = app


SOURCES += mazegui.cpp\
        main.cpp \
        micromouseserver.cpp \
    mazeBase.cpp \
    mazecell.cpp \
    cell.cpp


HEADERS  += micromouseserver.h \
    mazeConst.h \
    mazeBase.h \
    mazegui.h \
    mazecell.h \
    cell.h

FORMS    += micromouseserver.ui
