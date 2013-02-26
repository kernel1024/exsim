#ifndef CPLABEL_H
#define CPLABEL_H

#include <QtCore>
#include "cpbase.h"

class QCPLabel : public QCPBase
{
    Q_OBJECT
public:
    QString labelText;
    QColor labelColor;
    explicit QCPLabel(QWidget *parent, QRenderArea *aOwner);

    QSize minimumSizeHint() const;

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

protected:
    void paintEvent(QPaintEvent *event);
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void changeText();
    void changeColor();
};

#endif // CPLABEL_H
