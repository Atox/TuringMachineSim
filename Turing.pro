#-------------------------------------------------
#
# Project created by QtCreator 2013-10-14T19:50:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Turing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    commandsmodel.cpp \
    command.cpp \
    tape.cpp \
    machine.cpp \
    tapestablemodel.cpp

HEADERS  += mainwindow.h \
    commandsmodel.h \
    command.h \
    tape.h \
    machine.h \
    tapestablemodel.h

FORMS    += mainwindow.ui

RESOURCES =
