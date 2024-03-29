#ifndef CPGEN_H
#define CPGEN_H

#include "cpbase.h"
#include <QTimer>

class QCPGen : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;

    explicit QCPGen(QWidget *parent, QRenderArea *aOwner);
    ~QCPGen();

    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

    void setPeriod(int msec);

protected:
    int period;
    QTimer mainTimer;
    bool genState;
    void paintEvent(QPaintEvent *event);
    void realignPins();
    void doLogicPrivate();
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void adjustPeriod();
    void timeImpulse();

};

#endif // CPGEN_H
