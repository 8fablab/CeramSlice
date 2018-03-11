#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QVector>

#include "GL_Renderer.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public slots:
        void OpenGeometryFile();
        void Process();
        void setTotalHeightMin(double min);
        void UpdateStats();

    public:
        explicit MainWindow(QWidget *parent = 0);

        void UpdatePreview();

        void RemoveHeader();
        void RemoveFooter();
        void AddOpcode_G1();
        void GenerateLayers();

        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        QString GeometryFilePath;
        QFile *GeometryFile;

        QVector<QString> Header;

        QVector<QString> InitialData;
        QVector<QString> Data;

        QVector<QString> Footer;

        bool HeaderRemoved;
        bool FooterRemoved;
        bool Opcode_G1_Added;

        GL_Renderer *Renderer;


};

#endif // MAINWINDOW_H
