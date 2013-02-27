#ifndef CPTRIGGER_H
#define CPTRIGGER_H

#include "cpbase.h"

class QCPTrigger : public QCPBase
{
    Q_OBJECT
public:
    enum TriggerMode {
        TM_RSD=1,
        TM_RST=2
    };

    QCPOutput* fQOut;
    QCPOutput* fNQOut;
    QCPInput* fSInp;
    QCPInput* fDTInp;
    QCPInput* fCInp;
    QCPInput* fRInp;
    explicit QCPTrigger(QWidget *parent, QRenderArea *aOwner);
    ~QCPTrigger();
    QSize minimumSizeHint() const;

    void readFromStream(QTextStream &errlog, const QDomElement &element);
    void storeToStream(QDomElement &element);
    void setMode(TriggerMode tMode);

protected:
    bool state;
    bool savedT;
    TriggerMode mode;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void applyDtrigger();
    void applyTtrigger();

};

#endif // CPTRIGGER_H
