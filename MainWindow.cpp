#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <QTextBlock>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("CeramSlice <3");
    FooterRemoved = false;
    HeaderRemoved = false;
    Opcode_G1_Added = false;
    WorkerInProgress = false;

    ui->lbl_TotalHeight->hide();
    ui->TotalHeight_num->hide();

    Renderer = new GL_Renderer(60, this, "Part renderer");

    //ui->MDI_Area->hide();
    ui->Progress->hide();
    ui->MDI_Area->addSubWindow(Renderer);
    Renderer->showMaximized();
    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete GeometryFile;
    delete Renderer;
}

void MainWindow::UpdatePreview()
{
    QString WholeHeader;

    ui->preview->clear();

    if(Data.size() > 100000)
        ui->statusBar->showMessage(tr("Updating preview... Will take some time, don't panic !"));
    else
        ui->statusBar->showMessage(tr("Updating preview..."));

    ui->Progress->show();
    ui->Progress->setValue(0);

    for(int i = 0; i < Header.size(); i++)
    {
        WholeHeader.append(Header[i] + QString("\n"));
    }

    ui->preview->appendPlainText(WholeHeader);

    ui->Progress->setValue(10);

    if(Data.size() == 0)
    {
        QString WholeData;

        for(int i = 0; i < InitialData.size(); i++)
        {
            WholeData.append(InitialData[i] + QString("\n"));
        }

        ui->preview->appendPlainText(WholeData);

        ui->NumOfLine->setText(QString::number(Header.size()+InitialData.size()+Footer.size()));
    }

    else
    {
        QString WholeData;

        for(int i = 0; i < Data.size(); i++)
        {
            WholeData.append(Data[i] + QString("\n"));
        }

        ui->preview->appendPlainText(WholeData);

        ui->NumOfLine->setText(QString::number(Header.size()+Data.size()+Footer.size()));
    }

    ui->Progress->setValue(99);

    QString WholeFooter;

    for(int i = 0; i < Footer.size(); i++)
    {
        WholeFooter.append(Footer[i] + QString("\n"));
    }

    ui->preview->appendPlainText(WholeFooter);
    ui->Progress->setValue(0);

    ui->statusBar->clearMessage();
    ui->Progress->hide();
}

void MainWindow::UpdateStats()
{
    double LayerHeight = ui->Layer_num->value();
    double TotalHeight = ui->TotalHeight_num->value();

    int NumberOfLayers = TotalHeight / LayerHeight;

    ui->NumOfLayer_lbl->setText(QString::number(NumberOfLayers));
}

void MainWindow::SetNumberOfLayer(int Layers)
{
    ui->NumOfLayer_lbl->setText(QString::number(Layers));
}

void MainWindow::UpdateProgress(int Progress)
{
    ui->Progress->setValue(Progress);
}

void MainWindow::setTotalHeightMin(double min)
{
    ui->TotalHeight_num->setMinimum(min);
    ui->TotalHeight_num->setSingleStep(min);
}

void MainWindow::OpenGeometryFile()
{
    QRegExp NC_File("\\.[nN][cC]$");
    QRegExp PS_File("\\.[pP][sS]$");

    GeometryFilePath = QFileDialog::getOpenFileName(this, tr("Open Geometry File"), "", tr(""));
    ui->GeometryPath_ln->setText(GeometryFilePath);

    GeometryFile = new QFile(GeometryFilePath);

    if (!GeometryFile->open(QIODevice::ReadOnly | QIODevice::Text))
            return;

    QTextStream in(GeometryFile);
    ui->preview->clear();

    if(GeometryFilePath.contains(NC_File))
    {
        ui->lbl_TotalHeight->show();
        ui->TotalHeight_num->show();
    }

    else if(GeometryFilePath.contains(PS_File))
    {
        ui->lbl_TotalHeight->hide();
        ui->TotalHeight_num->hide();
    }

    FooterRemoved = false;
    HeaderRemoved = false;
    Opcode_G1_Added = false;

    Header.clear();
    InitialData.clear();
    Data.clear();
    Footer.clear();

    while (!in.atEnd())
    {
        InitialData.push_back(in.readLine());
    }

    UpdatePreview();
    UpdateStats();
}

void MainWindow::WorkerFinished()
{
    WorkerInProgress = false;
    ui->statusBar->clearMessage();
    ui->Preview_btn->setEnabled(true);
    ui->SelectFileGeom_btn->setEnabled(true);
    ui->Progress->hide();
}

void MainWindow::Process()
{
    QRegExp NC_File("\\.[nN][cC]$");
    QRegExp PS_File("\\.[pP][sS]$");

    if(ui->GeometryPath_ln->text().contains(NC_File))
    {
        ui->Progress->show();
        ui->Preview_btn->setEnabled(false);
        ui->SelectFileGeom_btn->setEnabled(false);
        ui->statusBar->showMessage(tr("Processing NC file..."));
        NC_Worker = new NC_ParserWorker(InitialData, ui->Layer_num->value(), ui->TotalHeight_num->value(), this);
        connect(NC_Worker, &NC_ParserWorker::progress, this, &MainWindow::UpdateProgress);
        connect(NC_Worker, &NC_ParserWorker::resultReady, this, &MainWindow::HandleParserResult);
        connect(NC_Worker, &NC_ParserWorker::finished, this, &MainWindow::WorkerFinished);
        connect(NC_Worker, &NC_ParserWorker::finished, NC_Worker, &QObject::deleteLater);
        NC_Worker->start();
    }

    else if(ui->GeometryPath_ln->text().contains(PS_File))
    {
        ui->Progress->show();
        ui->Preview_btn->setEnabled(false);
        ui->SelectFileGeom_btn->setEnabled(false);
        ui->statusBar->showMessage(tr("Processing PS file..."));

        PS_Worker = new PS_ParserWorker(InitialData, ui->Layer_num->value(), ui->TotalHeight_num->value(), this);
        connect(PS_Worker, &PS_ParserWorker::progress, this, &MainWindow::UpdateProgress);
        connect(PS_Worker, &PS_ParserWorker::updateNumLayers, this, &MainWindow::SetNumberOfLayer);
        connect(PS_Worker, &PS_ParserWorker::resultReady, this, &MainWindow::HandleParserResult);
        connect(PS_Worker, &PS_ParserWorker::finished, this, &MainWindow::WorkerFinished);
        connect(PS_Worker, &PS_ParserWorker::finished, PS_Worker, &QObject::deleteLater);
        PS_Worker->start();
    }

    else
        QMessageBox::critical(this, "Woops !", "Unknown file extension, cannot process...");
}


void MainWindow::HandleParserResult(const QVector<QString> &pHeader, const QVector<QString> &pData, const QVector<QString> &pFooter)
{
    Header = pHeader;
    Data = pData;
    Footer = pFooter;

    UpdatePreview();
}
