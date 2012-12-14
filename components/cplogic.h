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

protected:
    qint32 state;
    qint32 oldState;
    void realignPins(QPainter & painter);
    void doLogicPrivate();
    bool isStateChanged();
    void paintEvent(QPaintEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

signals:
    
public slots:
    void setModeAND();
    void setModeOR();
    void setModeXOR();
    void setModeNOT();
    void adjustInpCount();
    
};

#endif // CPLOGIC_H
