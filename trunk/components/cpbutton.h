#ifndef CPBUTTON_H
#define CPBUTTON_H

#include <cpbase.h>

class QCPButton : public QCPBase
{
    Q_OBJECT
public:
    QCPOutput* fOut;
    explicit QCPButton(QWidget *parent, QRenderArea *aOwner);
    ~QCPButton();

    QSize minimumSizeHint() const;

protected:
    bool oldPressed;
    bool pressed;
    QPoint savedClick;
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void realignPins(QPainter & painter);
    void doLogicPrivate();

signals:
    
public slots:
    
};

#endif // CPBUTTON_H
