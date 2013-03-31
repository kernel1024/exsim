#ifndef CPBEEPER_H
#define CPBEEPER_H

#include "cpbase.h"
#if WIN32
#include "AL/al.h"
#include "AL/alc.h"
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

class QCPBeeper : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* fInp;
    bool alError;
    bool alPlaying;
    explicit QCPBeeper(QWidget *parent, QRenderArea *aOwner);
    ~QCPBeeper();
    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

    void updateFreq(float aFreq);

protected:
    ALuint albuf;
    ALuint alsrc;

    float freq;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

public slots:
    void chooseFreq();
};

#endif // CPBEEPER_H
