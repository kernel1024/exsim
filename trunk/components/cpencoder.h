#ifndef CPENCODER_H
#define CPENCODER_H

#include "cpbase.h"

class QCPEncoder : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f4Out;
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f3Inp;
    QCPInput* f4Inp;
    QCPInput* f5Inp;
    QCPInput* f6Inp;
    QCPInput* f7Inp;

    explicit QCPEncoder(QWidget *parent, QRenderArea *aOwner);
    ~QCPEncoder();
    QSize minimumSizeHint() const;

protected:
    qint8 state;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPENCODER_H
