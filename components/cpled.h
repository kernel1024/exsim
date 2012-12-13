#ifndef CPLED_H
#define CPLED_H

#include <cpbase.h>

class QCPLed : public QCPBase
{
    Q_OBJECT
public:
    explicit QCPLed(QWidget *parent, QRenderArea *aOwner);
    
private:
    void realignPins(QPainter & painter);
    void doLogicPrivate();

signals:
    
public slots:
    
};

#endif // CPLED_H
