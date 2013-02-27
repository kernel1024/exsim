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

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

protected:
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void updateMuxPins();
    void contextMenuEvent(QContextMenuEvent *);

public slots:
    void changeAddrSize();
    void changeDataSize();
};

#endif // CPMUX_H