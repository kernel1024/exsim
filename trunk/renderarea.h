#ifndef RENDERAREA_H
#define RENDERAREA_H 1

#include <QtCore>
#include <QtGui>
#include "components/cpbase.h"

class QRenderArea : public QFrame
{
    Q_OBJECT
public:
    QRenderArea(QWidget *parent = 0, QScrollArea *aScroller=0);
    
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    
    QScrollArea *scroller;
    
    bool resReading, erroneousRoute, rectLinks;
    QStringList nodeLocks;
    QLabel* recycle;
    QDataStream* storeStream;
    
    int cbType, cbPinNum, cbConnCount;
    QCPInput *cbInput;
    QCPOutput *cbOutput;
    bool cbBuilding;
    QPoint cbCurrent;
    int zoom;
    
    void initConnBuilder(const int aType, int aPinNum, QCPInput* aInput, QCPOutput* aOutput);
    void repaintConn();
    void refreshConnBuilder(const QPoint & atPos);
    void doneConnBuilder(const bool aNone, int aType, const int aPinNum, QCPInput* aInput, QCPOutput* aOutput);
    void postLoadBinding();
    void readSchematic(QDataStream & stream);
    void storeSchematic(QDataStream & stream);
    void deleteComponents();
    int cpComponentCount();
    void setZoom(int zoomFactor);
    QCPBase* createCpInstance(const QString & className);
protected:
    void paintEvent ( QPaintEvent * event );
};

#endif
