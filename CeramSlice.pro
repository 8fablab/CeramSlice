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

#LIB_DIR = "glew-2.1.0"
#INCLUDEPATH += $$LIB_DIR/include
#LIBS += -L$$LIB_DIR/lib -lglew32

SOURCES += main.cpp\
        MainWindow.cpp \
    GL_Renderer.cpp \
    NC_ParserWorker.cpp \
    PS_ParserWorker.cpp \
    View/Shader.cpp \
    View/Camera.cpp

HEADERS  += MainWindow.h \
    GL_Renderer.h \
    NC_ParserWorker.h \
    PS_ParserWorker.h \
    View/Shader.h \
    View/Camera.h

FORMS    += MainWindow.ui
