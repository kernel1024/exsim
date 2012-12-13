#ifndef CPLOGIC_H
#define CPLOGIC_H

#include <cpbase.h>

class QCPLogic : public QCPBase
{
    Q_OBJECT
public:
    enum LogicType {
        LT_AND = 1,
        LT_OR = 2,
        LT_XOR = 3,
        LT_NOT = 4
    };

    LogicType mode;
    QCPOutput* fOut;
    explicit QCPLogic(QWidget *parent, QRenderArea *aOwner);
    ~QCPLogic();
    QSize minimumSizeHint() const;

    void readFromStream(QDataStream &stream);
    void storeToStream(QDataStream &stream);

    void setMode(LogicType lt, int inputCount);


private:
    qint32 state;
    qint32 oldState;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    bool isStateChanged();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    
public slots:
    
};

#endif // CPLOGIC_H
