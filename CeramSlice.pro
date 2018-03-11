#-------------------------------------------------
#
# Project created by QtCreator 2018-03-07T10:14:11
#
#-------------------------------------------------

QT       += core gui widgets opengl

TARGET = CeramSlice
TEMPLATE = app
LIBS += -lopengl32
LIBS += -lglu32

SOURCES += main.cpp\
        MainWindow.cpp \
    GL_Renderer.cpp

HEADERS  += MainWindow.h \
    GL_Renderer.h

FORMS    += MainWindow.ui
