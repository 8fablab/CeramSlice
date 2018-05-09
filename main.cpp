#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType< QVector<QString> >("QVector<QString>");
    MainWindow w;    
    w.show();

    return a.exec();
}
