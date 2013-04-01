#ifndef CPGND_H
#define CPGND_H

#include "cpbase.h"

class QCPGnd : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;

    explicit QCPGnd(QWidget *parent, QRenderArea *aOwner);
    ~QCPGnd();

    QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
    void realignPins();
    void doLogicPrivate();
};

#endif // CPGND_H
