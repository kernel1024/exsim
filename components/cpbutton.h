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

private:
    bool oldPressed;
    bool pressed;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    void mousePressEvent(QMouseEvent *event);
    bool isStateChanged();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    
public slots:
    
};

#endif // CPBUTTON_H
