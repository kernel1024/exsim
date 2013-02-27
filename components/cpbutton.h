#ifndef CPBUTTON_H
#define CPBUTTON_H

#include "cpbase.h"

class QCPButton : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;
    explicit QCPButton(QWidget *parent, QRenderArea *aOwner);
    ~QCPButton();

    QSize minimumSizeHint() const;

    void readFromXML(QTextStream &errlog, const QDomElement &element);
    void storeToXML(QDomElement &element);

protected:
    bool oldPressed;
    bool pressed;
    bool pushButton;
    QPoint savedClick;
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void contextMenuEvent(QContextMenuEvent *);

public slots:
    void modePushButton();
    void modeTumbler();
    
};

#endif // CPBUTTON_H
