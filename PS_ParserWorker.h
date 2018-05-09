#ifndef PS_PARSERWORKER_H
#define PS_PARSERWORKER_H

#include <QThread>
#include <QVector>
#include <QRegExp>

class PS_ParserWorker : public QThread
{
        Q_OBJECT
    public:
        explicit PS_ParserWorker(QVector<QString> &pInitialData, double pLayerHeight, double pTotalHeight, QObject *parent = 0);
        void run();

        void PS_GenerateLayers();
        void PS_ProcessData();
        void PS_RemoveFooter();
        void PS_RemoveHeader();

        int GetBlockNumber();
        QVector<QString> GetDataBlock(int Block);

    signals:
        void resultReady(const QVector<QString> &Header, const QVector<QString> &Data, const QVector<QString> &Footer);
        void progress(const int &Progress);
        void updateNumLayers(const int &Layers);

    public slots:

    private:
        bool HeaderRemoved;
        bool FooterRemoved;
        bool Opcode_G1_Added;
        bool LayerNumberRead;

        double TotalHeight;
        double LayerHeight;
        int LayerNumber;

        QVector<QString> Header;

        QVector<QString> InitialData;
        QVector<QString> Data;

        QVector<QString> Footer;
};

#endif // PS_PARSERWORKER_H
