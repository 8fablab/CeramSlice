#ifndef NC_PARSERWORKER_H
#define NC_PARSERWORKER_H

#include <QThread>
#include <QVector>
#include <QRegExp>

class NC_ParserWorker : public QThread
{
        Q_OBJECT
    public:
        explicit NC_ParserWorker(QVector<QString> &pInitialData, double pLayerHeight, double pTotalHeight, QObject *parent = 0);
        void run();

        void NC_GenerateLayers();
        void NC_AddOpcode_G1();
        void NC_RemoveFooter();
        void NC_RemoveHeader();

    signals:
        void resultReady(const QVector<QString> &Header, const QVector<QString> &Data, const QVector<QString> &Footer);
        void progress(const int &Progress);

    public slots:

    private:
        bool HeaderRemoved;
        bool FooterRemoved;
        bool Opcode_G1_Added;

        double TotalHeight;
        double LayerHeight;

        QVector<QString> Header;

        QVector<QString> InitialData;
        QVector<QString> Data;

        QVector<QString> Footer;
};

#endif // NC_PARSERWORKER_H
