#ifndef CPGEN_H
#define CPGEN_H

#include <QtCore>
#include "cpbase.h"

class QCPGen : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;

    explicit QCPGen(QWidget *parent, QRenderArea *aOwner);
    ~QCPGen();

    QSize minimumSizeHint() const;

    void readFromStream(QTextStream &errlog, const QDomElement &element);
    void storeToStream(QDomElement &element);

    void setPeriod(int msec);

protected:
    int period;
    QTimer mainTimer;
    bool genState;
    void paintEvent(QPaintEvent *event);
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void adjustPeriod();
    void timeImpulse();

};

#endif // CPGEN_H
