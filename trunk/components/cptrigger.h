#ifndef CPTRIGGER_H
#define CPTRIGGER_H

#include <cpbase.h>

class QCPTrigger : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fQOut;
    QCPOutput* fNQOut;
    QCPInput* fSInp;
    QCPInput* fDInp;
    QCPInput* fCInp;
    QCPInput* fRInp;
    explicit QCPTrigger(QWidget *parent, QRenderArea *aOwner);
    ~QCPTrigger();
    QSize minimumSizeHint() const;

protected:
    bool state;
    bool oldState;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    bool isStateChanged();
    void paintEvent(QPaintEvent *event);

};

#endif // CPTRIGGER_H
