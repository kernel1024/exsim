#ifndef CPDIGIT_H
#define CPDIGIT_H

#include "cpbase.h"

class QCPDigit : public QCPBase
{
    Q_OBJECT
public:
    QCPInput* f1Inp;
    QCPInput* f2Inp;
    QCPInput* f4Inp;
    QCPInput* f8Inp;
    explicit QCPDigit(QWidget *parent, QRenderArea *aOwner);
    ~QCPDigit();
    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

    void zoomChanged();

protected:
    QColor fontColor;
    int lcdFontSize;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *);
    void calcDigitSize();

public slots:
    void chooseColor();

};

#endif // CPDIGIT_H
