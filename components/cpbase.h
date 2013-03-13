#ifndef CPBASE_H
#define CPBASE_H 1

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QString>
#include <QPainter>
#include <QScrollArea>
#include <QDomElement>
#include <QDomDocument>
#include <QUuid>
#include <QTextStream>

#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QDebug>

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
    void moveComponent(QMouseEvent *event);

protected:
    float oldZoom;
    int zoom() const;
    QString savedState;
    bool isStateChanged();
    QString calcState();

    virtual void realignPins(QPainter & painter)=0;
    virtual void doLogicPrivate()=0;

    int getDCompHeight(const int divCount) const;
    int getDCompIncrement() const;
    void deleteInputs();
    void deleteOutputs();
    void drawSelection(QPainter & painter);

    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);

public:
    QCPBase(QWidget *parent, QRenderArea *aOwner);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void postLoadBind(QTextStream &errlog);
    void doLogic();
    void redrawPins(QPainter & painter);
    virtual void zoomChanged();
    int getPinSize() const;
    void deleteComponent();

    virtual void readFromXML(QTextStream &errlog, const QDomElement &element);
    virtual void storeToXML(QDomElement & element);
    virtual bool canConnectOut(const QCPBase * toComponent);
    virtual bool canConnectIn(const QCPBase * toComponent);
    void regroupOutputs();

    QRenderArea *cpOwner;
    QList<QCPInput*> fInputs;
    QList<QCPOutput*> fOutputs;
    QPoint relCorner;
    bool isDragging;
signals:
    void componentChanged(QCPBase * obj);
private slots:
    void applyInputState(QCPInput* input);
};

class QCPOutput : public QObject
{
    Q_OBJECT
public:
    QCPOutput(QObject * parent, QCPBase * aOwner, QString aPinName = QString());
    void readFromXML(QTextStream & errlog, const QDomElement &element);
    void storeToXML(QDomElement & element);
    void postLoadBind(QTextStream &errlog);
    void applyState();
    
    QCPBase *toCmp;
    QCPBase *ownerCmp;
    QPoint relCoord;
    qint32 toPin;
    QString pinName;
    bool inversed;
    QString ffLogic;

    QUuid groupId;

    bool state;
    bool oldState;
};

class QCPInput : public QObject
{
    Q_OBJECT
public:
    QCPBase * fromCmp;
    QCPBase * ownerCmp;
    QPoint relCoord;
    qint32 fromPin;
    QString pinName;
    bool inversed;
    QString ffLogic;
    bool state;
    bool oldState;

    QCPInput(QObject * parent, QCPBase * aOwner, QString aPinName = QString());

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement & element);
    void postLoadBind(QTextStream &errlog);
    void applyState(bool aState);

signals:
    void applyInputState(QCPInput* input);
};

int ipow(int base, int pow);

#endif
