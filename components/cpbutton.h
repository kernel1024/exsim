#ifndef CPBUTTON_H
#define CPBUTTON_H

#include <cpbase.h>

class QCPButton : public QCPBase
{
    Q_OBJECT
public:
    explicit QCPButton(QWidget *parent, QRenderArea *aOwner);

private:
    void realignPins(QPainter & painter);
    void doLogicPrivate();

signals:
    
public slots:
    
};

#endif // CPBUTTON_H
