#ifndef CPDECODER_H
#define CPDECODER_H

#include "cpbase.h"

class QCPDecoder : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* f1Out;
    QCPOutput* f2Out;
    QCPOutput* f3Out;
    QCPOutput* f4Out;
    QCPOutput* f5Out;
    QCPOutput* f6Out;
    QCPOutput* f7Out;
    QCPOutput* f8Out;
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f4Inp;
    QCPInput* fOEInp;

    explicit QCPDecoder(QWidget *parent, QRenderArea *aOwner);
    ~QCPDecoder();
    QSize minimumSizeHint() const;

protected:
    qint8 state;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPDECODER_H
