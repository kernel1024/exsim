#ifndef CPLED_H
#define CPLED_H

#include <cpbase.h>

class QCPLed : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* fInp;
    explicit QCPLed(QWidget *parent, QRenderArea *aOwner);
    ~QCPLed();
    QSize minimumSizeHint() const;

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);
    
private:
    QColor onColor;
    QColor offColor;
    bool state;
    bool oldState;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    bool isStateChanged();

protected:
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

signals:
    
public slots:
    void chooseColorOn();
    void chooseColorOff();
    
};

#endif // CPLED_H
