#ifndef RENDERAREA_H
#define RENDERAREA_H 1

#include <QFrame>
#include <QLabel>
#include <QRubberBand>
#include <QList>
#include <QStringList>
#include <QScrollArea>
#include <QTimer>

#include "components/cpbase.h"
class MainWindow;

class QRenderArea : public QFrame
{
    Q_OBJECT
public:
    QRenderArea(QWidget *parent = 0, QScrollArea *aScroller=0, int aLcdFontIdx = -1,
                MainWindow *aMainWindow = NULL);
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
    QScrollArea *scroller;
    MainWindow *mainWindow;

    QList<QUuid> pendingLogicLinks;
    
    bool resReading;
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
    QTimer* periodicCheckTimer;
    void paintEvent ( QPaintEvent * event );
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintConnections(QPainter *p);
};

#endif
