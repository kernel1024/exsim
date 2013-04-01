#ifndef CPLABEL_H
#define CPLABEL_H

#include "cpbase.h"

class QCPLabel : public QCPBase
{
    Q_OBJECT
public:
    QString labelText;
    QColor labelColor;
    QFont labelFont;
    explicit QCPLabel(QWidget *parent, QRenderArea *aOwner);

    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

protected:
    void paintEvent(QPaintEvent *event);
    void realignPins();
    void doLogicPrivate();
    void contextMenuEvent(QContextMenuEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void changeText();
    void changeColor();
    void changeFont();
};

#endif // CPLABEL_H
