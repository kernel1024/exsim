#ifndef CPMUX_H
#define CPMUX_H

#include "cpbase.h"

class QCPMux : public QCPBase
{
    Q_OBJECT
public:
    int dWidth;
    int dCount;
    explicit QCPMux(QWidget *parent, QRenderArea *aOwner);
    ~QCPMux();
    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

protected:
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void updateMuxPins();
    void contextMenuEvent(QContextMenuEvent *);

public slots:
    void changeAddrSize();
    void changeDataSize();
};

#endif // CPMUX_H
