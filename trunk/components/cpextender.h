#ifndef CPEXTENDER_H
#define CPEXTENDER_H

#include <cpbase.h>

class QCPExtender : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* fInp;

    explicit QCPExtender(QWidget *parent, QRenderArea *aOwner);
    ~QCPExtender();

    QSize minimumSizeHint() const;

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

    void setMode(int outputCount);

protected:
    bool state;
    bool oldState;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    bool isStateChanged();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void adjustOutCount();

signals:
    
public slots:
    
};

#endif // CPEXTENDER_H
