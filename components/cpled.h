#ifndef CPLED_H
#define CPLED_H

#include "cpbase.h"

class QCPLed : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* fInp;
    explicit QCPLed(QWidget *parent, QRenderArea *aOwner);
    ~QCPLed();
    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);
    
protected:
    QColor onColor;
    QColor offColor;
    void realignPins();
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

signals:
    
public slots:
    void chooseColorOn();
    void chooseColorOff();
    
};

#endif // CPLED_H
