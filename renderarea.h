#ifndef RENDERAREA_H
#define RENDERAREA_H 1

#include <QtCore>
#include <QtGui>
#include <QtXml>
#include "components/cpbase.h"

class QRenderArea : public QFrame
{
    Q_OBJECT
public:
    QRenderArea(QWidget *parent = 0, QScrollArea *aScroller=0, int aLcdFontIdx = -1);
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
    QScrollArea *scroller;

    QList<QUuid> pendingLogicLinks;
    
    bool resReading;
    QStringList nodeLocks;
    QLabel* recycle;
    QPoint selectionPoint;
    QRubberBand* selectionBox;
    QList<QCPBase *> selection;
    
    int cbType, cbPinNum, cbConnCount;
    QCPInput *cbInput;
    QCPOutput *cbOutput;
    bool cbBuilding;
    QPoint cbCurrent;
    int zoom;

    QFont lcdFont;
    QColor pinColorOff;
    QColor pinColorOn;

    void initConnBuilder(const int aType, int aPinNum, QCPInput* aInput, QCPOutput* aOutput);
    void repaintConn();
    void refreshConnBuilder(const QPoint & atPos);
    void doneConnBuilder(const bool aNone, int aType, const int aPinNum, QCPInput* aInput, QCPOutput* aOutput);
    void postLoadBinding(QTextStream &errlog);
    void readSchematic(QTextStream &errlog, const QDomElement &element, const QPoint indent = QPoint(0,0),
                       const bool addBlock = false);
    void storeSchematic(QDomElement & element);
    void deleteComponents();
    int cpComponentCount();
    void setZoom(int zoomFactor);
    QCPBase* createCpInstance(const QString & className);
protected:
    void paintEvent ( QPaintEvent * event );
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintConnections(QPainter *p);
};

#endif
