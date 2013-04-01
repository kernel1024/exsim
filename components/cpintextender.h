#ifndef CPINTEXTENDER_H
#define CPINTEXTENDER_H

#include "cpbase.h"

class QCPIntExtender : public QCPBase
{
    Q_OBJECT
public:
    int dWidth;
    int dCount;
    explicit QCPIntExtender(QWidget *parent, QRenderArea *aOwner);
    ~QCPIntExtender();
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
    void changeChannelsCount();
    void changeDataSize();
};

#endif // CPINTEXTENDER_H
