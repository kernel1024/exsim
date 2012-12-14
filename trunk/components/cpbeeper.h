#ifndef CPBEEPER_H
#define CPBEEPER_H

#include <cpbase.h>
#include <AL/al.h>
#include <AL/alc.h>

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

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

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
