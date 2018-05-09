#include "NC_ParserWorker.h"

NC_ParserWorker::NC_ParserWorker(QVector<QString> &pInitialData, double pLayerHeight, double pTotalHeight, QObject *parent) : QThread(parent)
{
    InitialData = pInitialData;
    LayerHeight = pLayerHeight;
    TotalHeight = pTotalHeight;

    HeaderRemoved = false;
    FooterRemoved = false;
    Opcode_G1_Added = false;
}



void NC_ParserWorker::run()
{
    if(!HeaderRemoved)
        NC_RemoveHeader();

    if(!FooterRemoved)
        NC_RemoveFooter();

    if(!Opcode_G1_Added)
        NC_AddOpcode_G1();

    NC_GenerateLayers();

    emit resultReady(Header, Data, Footer);
}


void NC_ParserWorker::NC_RemoveHeader()
{
    QRegExp InitialPosition("(^G0 X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");
    QRegExp StartOfInitialData("(^X-?\\d+\\.\\d+$)|(^Y-?\\d+\\.\\d+$)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");

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


    emit progress(100);
}


void NC_ParserWorker::NC_RemoveFooter()
{
    QRegExp StartOfInitialData("(^X-?\\d+\\.\\d+$)|(^Y-?\\d+\\.\\d+$)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+$)");


    emit progress(0);

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

    FooterRemoved = true;


    emit progress(100);

}

void NC_ParserWorker::NC_AddOpcode_G1()
{
    QRegExp GCodeInstruction("^G\\d+ ");

    emit progress(0);

    for(int i = 0; i < InitialData.size(); i++)
    {
        if(!InitialData[i].contains(GCodeInstruction))
            InitialData[i].insert(0, "G1 ");
    }

    Footer.push_back("G92 E0");
    Footer.push_back("G28");

    Opcode_G1_Added = true;

    emit progress(100);
}

void NC_ParserWorker::NC_GenerateLayers()
{

    int NumberOfLayers = TotalHeight / LayerHeight;
    double Progress = 0;

    Data.clear();

    emit progress(0);

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
        Progress = 100.0*((double)i+1)/(double)NumberOfLayers;
        emit progress(Progress);
    }

    emit progress(0);
}
