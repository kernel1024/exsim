#ifndef CPCOUNTER_H
#define CPCOUNTER_H

#include "cpbase.h"

class QCPCounter : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f4Out;
    QCPOutput* f8Out;
    QCPOutput* fUFOut;
    QCPOutput* fOFOut;
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f4Inp;
    QCPInput* f8Inp;
    QCPInput* fCInp;
    QCPInput* fRInp;
    QCPInput* fIncInp;
    QCPInput* fDecInp;
    QCPInput* fModeInp;

    explicit QCPCounter(QWidget *parent, QRenderArea *aOwner);
    ~QCPCounter();
    QSize minimumSizeHint() const;

protected:
    qint8 state;
    bool savedC;
    bool savedInc;
    bool savedDec;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPCOUNTER_H
