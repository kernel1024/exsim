#ifndef CPSYNTH_H
#define CPSYNTH_H

#include "cpbase.h"
#include "openal.h"
#include <QMutex>

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
    QMutex stateUpdate;

    float freq;
    int savedCode;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void periodicCheck();
    bool checkTimerNeeded();
    void drawRotatedText(QPainter *painter, float degrees, const QRect& rect, const QString &text);
};

#endif // CPSYNTH_H
