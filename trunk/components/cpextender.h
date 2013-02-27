#ifndef CPEXTENDER_H
#define CPEXTENDER_H

#include "cpbase.h"

class QCPExtender : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* fInp;

    explicit QCPExtender(QWidget *parent, QRenderArea *aOwner);
    ~QCPExtender();

    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

    void setMode(int outputCount);

protected:
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void adjustOutCount();

signals:
    
public slots:
    
};

#endif // CPEXTENDER_H
