#include "cplogic.h"

QCPLogic::QCPLogic(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    mode = LT_AND;
    fOut = new QCPOutput(this,this);
    fOutputs.append(fOut);
    fInputs << new QCPInput(this,this);
    fInputs << new QCPInput(this,this);
}

QCPLogic::~QCPLogic()
{
    for(int i=0;i<fInputs.count();i++)
        fInputs.at(i)->deleteLater();
    fInputs.clear();
    fOutputs.clear();
    delete fOut;
}

QSize QCPLogic::minimumSizeHint() const
{
    int vsz = (QApplication::fontMetrics().height()+4)*(fInputs.count()+1);
    if (vsz<60) vsz = 60;
    return QSize(60,vsz);
}

void QCPLogic::readFromStream(QDataStream &stream)
{
    QCPBase::readFromStream(stream);
    int inpcnt, lt;
    stream >> lt >> inpcnt;
    setMode((LogicType)lt, inpcnt);
}

void QCPLogic::storeToStream(QDataStream &stream)
{
    QCPBase::storeToStream(stream);
    int inpcnt = fInputs.count();
    stream << (int)mode << inpcnt;
}

void QCPLogic::setMode(QCPLogic::LogicType lt, int inputCount)
{
    int icnt = inputCount;
    mode = lt;
    // check inputs count for certain logic types
    if (lt==LT_XOR)
        icnt = 2;
    if (lt==LT_NOT)
        icnt = 1;
    if (icnt>32)
        icnt=32;

    // disconnect and delete all inputs
    for (int i=0;i<fInputs.count();i++) {
        QCPInput* cbInput = fInputs.at(i);
        if ((cbInput->fromPin!=-1) && (cbInput->fromCmp!=0))
        {
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toCmp=0;
            cbInput->fromCmp->fOutputs[cbInput->fromPin]->toPin=-1;
        }
        cbInput->fromCmp=0;
        cbInput->fromPin=-1;
        cbInput->deleteLater();
    }
    fInputs.clear();

    // create new inputs
    for(int i=0;i<icnt;i++)
        fInputs << new QCPInput(this,this);

    update();
}

void QCPLogic::realignPins(QPainter &)
{
    fOut->relCoord=QPoint(width()-getPinSize()/2,height()/2);
    int dy = height()/(fInputs.count()+1);
    for(int i=0;i<fInputs.count();i++)
        fInputs[i]->relCoord=QPoint(getPinSize()/2,i*dy+dy);
}

void QCPLogic::doLogicPrivate()
{
    qint32 a = 0;
    for(int i=0;i<fInputs.count();i++)
        if (fInputs.at(i)->state)
            a = a | (1 << i);
    state = a;
    bool outv = false;
    switch (mode) {
        case LT_AND:
            outv = (state == ((1 << fInputs.count())-1));
            break;
        case LT_OR:
            outv = (state != 0);
            break;
        case LT_XOR:
            outv = (state==1 || state==2);
            break;
        case LT_NOT:
            outv = (state==0);
            break;
        default:
            state = 0;
            outv = false;
            break;
    }
    fOut->state = outv;
    oldState = state;
}

bool QCPLogic::isStateChanged()
{
    return (oldState != state);
}

void QCPLogic::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen op=p.pen();
    QBrush ob=p.brush();
    QFont of=p.font();

    p.setPen(QPen(Qt::black));
    p.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    QRect rc = rect();
    rc.adjust(0,0,-1,-1);
    p.drawRect(rc);

    redrawPins(p);

    QString s;
    switch (mode) {
        case LT_AND:
            s = "&";
            break;
        case LT_OR:
            s = "1";
            break;
        case LT_XOR:
            s = "=1";
            break;
        case LT_NOT:
            s = "~";
            break;
        default:
            break;
    }
    rc = rect();
    rc.adjust(0,0,0,-2*rc.height()/3);
    QFont n=of;
    p.setPen(QPen(Qt::black));
    n.setBold(true);
    n.setPointSize(n.pointSize()+2);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,s);

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

}
