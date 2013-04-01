#ifndef CPCOMPARATOR_H
#define CPCOMPARATOR_H

#include "cpbase.h"

class QCPComparator : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fLTOut;
    QCPOutput* fEQOut;
    QCPOutput* fGTOut;
    QCPInput* fX1Inp;
    QCPInput* fX2Inp;
    QCPInput* fX4Inp;
    QCPInput* fX8Inp;
    QCPInput* fY1Inp;
    QCPInput* fY2Inp;
    QCPInput* fY4Inp;
    QCPInput* fY8Inp;
    QCPInput* fOEInp;

    explicit QCPComparator(QWidget *parent, QRenderArea *aOwner);
    ~QCPComparator();
    QSize minimumSizeHint() const;

protected:
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPCOMPARATOR_H
