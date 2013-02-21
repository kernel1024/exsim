#ifndef CPSYNTH_H
#define CPSYNTH_H

#include "cpbase.h"
#include <AL/al.h>
#include <AL/alc.h>

class QCPSynth : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f4Inp;
    QCPInput* f8Inp;
    QCPInput* f16Inp;
    QCPInput* fOEInp;
    bool alError;
    bool alPlaying;
    explicit QCPSynth(QWidget *parent, QRenderArea *aOwner);
    ~QCPSynth();
    QSize minimumSizeHint() const;

    void updateFreq(int code);

protected:
    ALuint albuf;
    ALuint alsrc;

    float freq;
    int savedCode;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
};

#endif // CPSYNTH_H
