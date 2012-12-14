#ifndef CPBASE_H
#define CPBASE_H 1

#include <QtCore>
#include <QtGui>

#define QPT_INPUT     1
#define QPT_OUTPUT    2

class QCPInput;
class QCPOutput;
class QRenderArea;

class QCPBase : public QWidget
{
    Q_OBJECT

private:
    void mouseInPin(const QPoint & mx, int &aPinNum, int &aPinType, QCPBase * &aFilter);
    void checkRecycle();

protected:
    float oldZoom;
    int zoom() const;
    virtual void realignPins(QPainter & painter)=0;
    virtual void doLogicPrivate()=0;
    virtual bool isStateChanged()=0;

    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);
    
public:
    QCPBase(QWidget *parent, QRenderArea *aOwner);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void postLoadBind();
    void doLogic(bool forceUpdate = false);
    void redrawPins(QPainter & painter);
    void zoomChanged();
    int getPinSize();

    virtual void readFromStream( QDataStream & stream );
    virtual void storeToStream( QDataStream & stream );
    virtual bool canConnectOut(QCPBase * toFilter);
    virtual bool canConnectIn(QCPBase * toFilter);

    QRenderArea *cpOwner;
    QList<QCPInput*> fInputs;
    QList<QCPOutput*> fOutputs;
    QPoint relCorner;
    bool isDragging;
    QColor pinColorOff;
    QColor pinColorOn;
signals:
    void componentChanged(QCPBase * obj);
};

class QCPOutput : public QObject
{
    Q_OBJECT
public:
    QCPOutput(QObject * parent, QCPBase * aOwner);
    void readFromStream( QDataStream & stream );
    void storeToStream( QDataStream & stream );
    void postLoadBind();
    void applyState();
    
    QCPBase *toCmp;
    QCPBase *ownerCmp;
    QPoint relCoord;
    qint32 toPin;
    QString pinName;
    bool inversed;
    QString ffLogic;

    bool state;
    bool oldState;
};

class QCPInput : public QObject
{
    Q_OBJECT
public:
    QCPInput(QObject * parent, QCPBase * aOwner);
    
    void readFromStream( QDataStream & stream );
    void storeToStream( QDataStream & stream );
    void postLoadBind();
    void applyState(bool aState);

    QCPBase * fromCmp;
    QCPBase * ownerCmp;
    QPoint relCoord;
    qint32 fromPin;
    QString pinName;
    bool inversed;
    QString ffLogic;

    bool state;
    bool oldState;
};

#endif
