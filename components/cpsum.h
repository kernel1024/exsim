#ifndef CPSUM_H
#define CPSUM_H

#include "cpbase.h"

class QCPSum : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f4Out;
    QCPOutput* f8Out;
    QCPOutput* fOFOut;
    QCPInput* fX1Inp;
    QCPInput* fX2Inp;
    QCPInput* fX4Inp;
    QCPInput* fX8Inp;
    QCPInput* fY1Inp;
    QCPInput* fY2Inp;
    QCPInput* fY4Inp;
    QCPInput* fY8Inp;
    QCPInput* fOFInp;

    explicit QCPSum(QWidget *parent, QRenderArea *aOwner);
    ~QCPSum();
    QSize minimumSizeHint() const;

protected:
    quint8 state;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPSUM_H
