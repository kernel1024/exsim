#ifndef CPREPEATER_H
#define CPREPEATER_H

#include <cpbase.h>

class QCPRepeater : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;
    QCPInput* fInp;

    explicit QCPRepeater(QWidget *parent, QRenderArea *aOwner);
    ~QCPRepeater();

    QSize minimumSizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    void realignPins(QPainter & painter);
    void doLogicPrivate();

signals:
    void sendDelayedUpdate(bool state);

public slots:
    void delayedUpdate(bool state);
};

#endif // CPREPEATER_H
