#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QDebug>
#include <QVector3D>

#include "GL_Renderer.h"
#include "NC_ParserWorker.h"
#include "PS_ParserWorker.h"

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
        void SetNumberOfLayer(int Layers);
        void UpdateProgress(int Progress);
        void HandleParserResult(const QVector<QString> &pHeader, const QVector<QString> &pData, const QVector<QString> &pFooter);
        void WorkerFinished();
        void UpdateLayerView();
        virtual void timeOutSlot();

    public:
        explicit MainWindow(QWidget *parent = 0);

        void UpdatePreview();
        void Update3DView();

        // *.ps file parser
        void PS_RemoveHeader();
        void PS_RemoveFooter();
        void PS_AddOpcode_G1();
        void PS_GenerateLayers();

        // *.nc file parser
        void NC_RemoveHeader();
        void NC_RemoveFooter();
        void NC_AddOpcode_G1();
        void NC_GenerateLayers();

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
        bool FileParsed;

        GL_Renderer *Renderer;
        QTimer *t_Timer;

        bool WorkerInProgress;
        NC_ParserWorker *NC_Worker;
        PS_ParserWorker *PS_Worker;


};

#endif // MAINWINDOW_H
