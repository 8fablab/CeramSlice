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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdatePreview()
{
    ui->preview->clear();

    for(int i = 0; i < Header.size(); i++)
    {
        ui->preview->append(Header[i]);
    }

    if(Data.size() == 0)
    {
        for(int i = 0; i < InitialData.size(); i++)
        {
            ui->preview->append(InitialData[i]);
        }

        ui->NumOfLine->setText(QString::number(Header.size()+InitialData.size()+Footer.size()));
    }

    else
    {
        for(int i = 0; i < Data.size(); i++)
        {
            ui->preview->append(Data[i]);
        }

        ui->NumOfLine->setText(QString::number(Header.size()+Data.size()+Footer.size()));
    }

    for(int i = 0; i < Footer.size(); i++)
    {
        ui->preview->append(Footer[i]);
    }

}


void MainWindow::setTotalHeightMin(double min)
{
    ui->TotalHeight_num->setMinimum(min);
    ui->TotalHeight_num->setSingleStep(min);
}

void MainWindow::OpenGeometryFile()
{
    GeometryFilePath = QFileDialog::getOpenFileName(this, tr("Open Geometry File"), "", tr(""));
    ui->GeometryPath_ln->setText(GeometryFilePath);

    GeometryFile = new QFile(GeometryFilePath);

    if (!GeometryFile->open(QIODevice::ReadOnly | QIODevice::Text))
            return;

    QTextStream in(GeometryFile);
    ui->preview->clear();

    FooterRemoved = false;
    HeaderRemoved = false;
    Opcode_G1_Added = false;

    Header.clear();
    InitialData.clear();
    Footer.clear();

    while (!in.atEnd())
    {
        InitialData.push_back(in.readLine());
    }

    UpdatePreview();
    UpdateStats();
}

void MainWindow::Process()
{
    if(!HeaderRemoved)
        RemoveHeader();

    if(!FooterRemoved)
        RemoveFooter();

    if(!Opcode_G1_Added)
        AddOpcode_G1();

    GenerateLayers();

    UpdatePreview();
}

void MainWindow::RemoveHeader()
{
    QRegExp InitialPosition("(^G0 X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");
    QRegExp StartOfInitialData("(^X-?\\d+\\.\\d+$)|(^Y-?\\d+\\.\\d+$)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");

    ui->Progress->setValue(0);

    for(int LineNumber = 0; LineNumber < InitialData.size(); LineNumber++)
    {
        if(InitialData[LineNumber].contains(StartOfInitialData))
        {
            break;
        }

        else if(InitialData[LineNumber].contains(InitialPosition))
        {
            Header.push_back(InitialData[LineNumber]);
            InitialData.remove(LineNumber);
            LineNumber--;
        }

        else
        {
            InitialData.remove(LineNumber);
            LineNumber--;
        }
    }

    Header.push_front("M82 ; use absolute distances for extrusion");
    Header.push_front("G90 ; use absolute coordinates");
    Header.push_front("G21 ; set units to millimeters");
    Header.push_front("M 220 S30");
    Header.push_front("G1 Z200 F 3600");
    Header.push_front("G90");
    Header.push_front("G21");
    Header.push_front("G28");
    Header.push_front("");
    Header.push_front(";Generated with CeramSlice");


    HeaderRemoved = true;


    ui->Progress->setValue(100);
}


void MainWindow::RemoveFooter()
{
    QRegExp StartOfInitialData("(^X-?\\d+\\.\\d+$)|(^Y-?\\d+\\.\\d+$)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");


    ui->Progress->setValue(0);

    for(int LineNumber = InitialData.size()-1; LineNumber >= 0; LineNumber--)
    {
        if(InitialData[LineNumber].contains(StartOfInitialData))
        {
            break;
        }

        else
        {
            InitialData.remove(LineNumber);
        }
    }

    UpdatePreview();

    FooterRemoved = true;


    ui->Progress->setValue(100);

}

void MainWindow::AddOpcode_G1()
{
    QRegExp GCodeInstruction("^G\\d+ ");

    ui->Progress->setValue(0);

    for(int i = 0; i < InitialData.size(); i++)
    {
        if(!InitialData[i].contains(GCodeInstruction))
            InitialData[i].insert(0, "G1 ");
    }

    Footer.push_back("G92 E0");
    Footer.push_back("G28");

    UpdatePreview();

    Opcode_G1_Added = true;

    ui->Progress->setValue(100);
}

void MainWindow::UpdateStats()
{
    double LayerHeight = ui->Layer_num->value();
    double TotalHeight = ui->TotalHeight_num->value();

    int NumberOfLayers = TotalHeight / LayerHeight;

    ui->NumOfLayer_lbl->setText(QString::number(NumberOfLayers));
}

void MainWindow::GenerateLayers()
{
    double LayerHeight = ui->Layer_num->value();
    double TotalHeight = ui->TotalHeight_num->value();

    int NumberOfLayers = TotalHeight / LayerHeight;

    Data.clear();

    ui->Progress->setValue(0);

    for(int i = 0; i < NumberOfLayers; i++)
    {
        for(int j = 0; j < InitialData.size(); j++)
        {
            QString newData = InitialData[j];

            if(i == 0)
            {
                newData.append(" Z");
                newData.append(QString::number(LayerHeight, 'f', 3));
                Data.push_back(newData);
            }

            else
            {
                newData.append(" Z");
                newData.append(QString::number(i*LayerHeight+LayerHeight*j/InitialData.size(), 'f', 3));
                Data.push_back(newData);
            }
        }
        ui->Progress->setValue(i/NumberOfLayers);
        qApp->processEvents();
    }

    UpdatePreview();
}
