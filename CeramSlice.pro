#-------------------------------------------------
#
# Project created by QtCreator 2018-03-07T10:14:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CeramSlice
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    GL_Renderer.cpp

HEADERS  += MainWindow.h \
    GL_Renderer.h

FORMS    += MainWindow.ui
