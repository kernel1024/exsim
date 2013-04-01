#ifndef CPVCC_H
#define CPVCC_H

#include "cpbase.h"

class QCPVcc : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;

    explicit QCPVcc(QWidget *parent, QRenderArea *aOwner);
    ~QCPVcc();

    QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *event);
    void realignPins();
    void doLogicPrivate();
};

#endif // CPVCC_H
