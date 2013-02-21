#ifndef CPRNDGEN_H
#define CPRNDGEN_H

#include "cpbase.h"

class QCPRndGen : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f4Out;
    QCPOutput* f8Out;
    QCPInput* fCInp;
    QCPInput* fRInp;

    explicit QCPRndGen(QWidget *parent, QRenderArea *aOwner);
    ~QCPRndGen();
    QSize minimumSizeHint() const;

protected:
    qint8 state;
    bool savedC;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPRNDGEN_H
