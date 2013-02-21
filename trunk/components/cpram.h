#ifndef CPRAM_H
#define CPRAM_H

#include "cpbase.h"

class QCPRAM : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fD0Out;
    QCPOutput* fD1Out;
    QCPOutput* fD2Out;
    QCPOutput* fD3Out;
    QCPOutput* fD4Out;
    QCPOutput* fD5Out;
    QCPOutput* fD6Out;
    QCPOutput* fD7Out;
    QCPInput* fD0Inp;
    QCPInput* fD1Inp;
    QCPInput* fD2Inp;
    QCPInput* fD3Inp;
    QCPInput* fD4Inp;
    QCPInput* fD5Inp;
    QCPInput* fD6Inp;
    QCPInput* fD7Inp;
    QCPInput* fWInp;
    QCPInput* fOEInp;

    explicit QCPRAM(QWidget *parent, QRenderArea *aOwner);
    ~QCPRAM();
    QSize minimumSizeHint() const;

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

protected:
    QByteArray data;
    int addrWidth;
    bool savedW;
    void updateInputsCount();
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

public slots:
    void loadFromFile();
    void saveToFile();
    void changeSize();
    void resetData();
};

#endif // CPRAM_H
