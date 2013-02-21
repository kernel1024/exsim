#ifndef CPMUX_H
#define CPMUX_H

#include "cpbase.h"

class QCPMux : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fXOut;
    QCPOutput* fYOut;
    QCPInput* fX1Inp;
    QCPInput* fX2Inp;
    QCPInput* fX3Inp;
    QCPInput* fX4Inp;
    QCPInput* fY1Inp;
    QCPInput* fY2Inp;
    QCPInput* fY3Inp;
    QCPInput* fY4Inp;
    QCPInput* fA0Inp;
    QCPInput* fA1Inp;

    explicit QCPMux(QWidget *parent, QRenderArea *aOwner);
    ~QCPMux();
    QSize minimumSizeHint() const;

protected:
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPMUX_H
