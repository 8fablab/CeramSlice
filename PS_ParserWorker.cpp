#include "PS_ParserWorker.h"
#include <QMessageBox>

PS_ParserWorker::PS_ParserWorker(QVector<QString> &pInitialData, double pLayerHeight, double pTotalHeight, QObject *parent) : QThread(parent)
{
    InitialData = pInitialData;
    LayerHeight = pLayerHeight;
    TotalHeight = pTotalHeight;
    LayerNumber = 0;
    LayerNumberRead = false;
}



void PS_ParserWorker::run()
{
    if(!HeaderRemoved)
        PS_RemoveHeader();

    if(!FooterRemoved)
        PS_RemoveFooter();

    if(!Opcode_G1_Added)
        PS_ProcessData();

    PS_GenerateLayers();

    emit resultReady(Header, Data, Footer);
}


void PS_ParserWorker::PS_RemoveHeader()
{
    QRegExp StartOfInitialData("^-?\\d+\\.\\d+ -?\\d+\\.\\d+");

    emit progress(0);

    for(int LineNumber = 0; LineNumber < InitialData.size(); LineNumber++)
    {
        if(InitialData[LineNumber].contains(StartOfInitialData))
        {
            break;
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


void PS_ParserWorker::PS_RemoveFooter()
{
    QRegExp StartOfInitialData("^-?\\d+\\.\\d+ -?\\d+\\.\\d+");


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

void PS_ParserWorker::PS_ProcessData()
{
    QRegExp Data("^-?\\d+\\.\\d+ -?\\d+\\.\\d+");
    QRegExp FourthDigit("\\d ");
    QRegExp PS_DataOpcode(" [lm]$");
    QRegExp YData_Start(" -?\\d+\\.\\d+$");

    emit progress(0);
    LayerNumber = GetBlockNumber();
    emit updateNumLayers(LayerNumber);
    LayerNumberRead = true;

    for(int i = 0; i < InitialData.size(); i++)
    {
        if(InitialData[i].contains(Data))
        {
            InitialData[i].replace(FourthDigit, QString(" "));
            InitialData[i].remove(PS_DataOpcode);
            InitialData[i].insert(0, 'X');
            InitialData[i].insert(InitialData[i].indexOf(YData_Start)+1, 'Y');
        }
    }

    Footer.push_back("G92 E0");
    Footer.push_back("G28");

    Opcode_G1_Added = true;

    emit progress(100);
}

int PS_ParserWorker::GetBlockNumber()
{
    int NumOfBlock = 0;

    if(LayerNumberRead)
        return LayerNumber;

    QRegExp NewPage("^n$");
    QRegExp PS_Data("(^-?\\d+\\.\\d+ -?\\d+\\.\\d+)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+)");

    for(int i = 0; i < InitialData.size(); i++)
    {
        if(InitialData[i].contains(NewPage))
        {
            if(InitialData[i+1].contains(PS_Data))
            {
                //Start of a block
                NumOfBlock++;
            }
        }
    }

    return NumOfBlock;
}

QVector<QString> PS_ParserWorker::GetDataBlock(int Block)
{
    int CurrentBlock = 1;
    QVector<QString> BlockData;
    QRegExp NewPage("^n$");
    QRegExp PS_Data("(^-?\\d+\\.\\d+ -?\\d+\\.\\d+)|(^X-?\\d+\\.\\d+ Y-?\\d+\\.\\d+)");

    for(int i = 0; i < InitialData.size(); i++)
    {
        if(InitialData[i].contains(NewPage) || (i == 0 && HeaderRemoved))
        {
            if(InitialData[i+1].contains(PS_Data))
            {
                //Start of a block
                if(CurrentBlock == Block+1)
                {
                    for(int j = i+1; j < InitialData.size() && InitialData[j].contains(PS_Data); j++)
                    {
                        BlockData.push_back(InitialData[j]);
                    }

                    return BlockData;
                }

                CurrentBlock++;
            }
        }
    }

    return BlockData;
}

void PS_ParserWorker::PS_GenerateLayers()
{
    double Progress = 0;

    Data.clear();

    emit progress(0);

    for(int i = 0; i <= LayerNumber; i++)
    {
        QVector<QString> Block = GetDataBlock(i);

        for(int j = 0; j < Block.size(); j++)
        {
            QString newData;

            if(i == 0)
            {
                newData.append("G1 ");
                newData.append(Block[j]);
                newData.append(" Z");
                newData.append(QString::number(LayerHeight, 'f', 3));
                Data.push_back(newData);
            }

            else
            {
                newData.append("G1 ");
                newData.append(Block[j]);
                newData.append(" Z");
                newData.append(QString::number((double)i*(double)LayerHeight+(double)LayerHeight*(double)j/(double)Block.size(), 'f', 3));
                Data.push_back(newData);
            }
        }

        Progress = 100.0*((double)i+1)/(double)LayerNumber;
        emit progress(Progress);

    }

    emit progress(0);
}
