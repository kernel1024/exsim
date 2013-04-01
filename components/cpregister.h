#ifndef CPREGISTER_H
#define CPREGISTER_H

#include "cpbase.h"

class QCPRegister : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f4Out;
    QCPOutput* f8Out;
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f4Inp;
    QCPInput* f8Inp;
    QCPInput* fCInp;
    QCPInput* fRInp;
    QCPInput* fLSInp;
    QCPInput* fRSInp;

    explicit QCPRegister(QWidget *parent, QRenderArea *aOwner);
    ~QCPRegister();
    QSize minimumSizeHint() const;

protected:
    qint8 state;
    bool savedC;
    bool savedLS;
    bool savedRS;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPREGISTER_H
