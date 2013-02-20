#include "cpdigit.h"
#include "../renderarea.h"

QCPDigit::QCPDigit(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fontColor = Qt::green;
    f1Inp = new QCPInput(this,this,"1"); fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2"); fInputs.append(f2Inp);
    f4Inp = new QCPInput(this,this,"4"); fInputs.append(f4Inp);
    f8Inp = new QCPInput(this,this,"8"); fInputs.append(f8Inp);
    int h = 3*minimumSizeHint().height()/4;
    lcdFontSize=cpOwner->lcdFont.pointSize();
    for(int i=0;i<100;i++) {
        QFont n=cpOwner->lcdFont;
        n.setPointSize(cpOwner->lcdFont.pointSize()+i);
        QFontMetrics fm(n);
        if (fm.height()>h) break;
        lcdFontSize=cpOwner->lcdFont.pointSize()+i;
    }
}

QCPDigit::~QCPDigit()
{
    fInputs.clear();
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete f8Inp;
}

QSize QCPDigit::minimumSizeHint() const
{
    int h = getDCompHeight(0);
    if (h<100) h=100;
    return QSize(80*zoom()/100,
                 h*zoom()/100);
}

void QCPDigit::readFromStream(QDataStream &stream)
{
    QCPBase::readFromStream(stream);
    stream >> fontColor;
}

void QCPDigit::storeToStream(QDataStream &stream)
{
    QCPBase::storeToStream(stream);
    stream << fontColor;
}

void QCPDigit::realignPins(QPainter &)
{
    int dy=getDCompIncrement();
    f1Inp->relCoord=QPoint(getPinSize()/2,dy);
    f2Inp->relCoord=QPoint(getPinSize()/2,2*dy);
    f4Inp->relCoord=QPoint(getPinSize()/2,3*dy);
    f8Inp->relCoord=QPoint(getPinSize()/2,4*dy);
}

void QCPDigit::doLogicPrivate()
{
}

void QCPDigit::paintEvent(QPaintEvent *)
{
    qint8 di = ((f8Inp->state << 3) & 0x8) |
               ((f4Inp->state << 2) & 0x4) |
               ((f2Inp->state << 1) & 0x2) |
               (f1Inp->state & 0x1);

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

    rc = rect();
    rc.adjust(p.fontMetrics().width("DM")+getPinSize()/2,0,-1,-1);
    p.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    p.drawRect(rc);

    QFont n = cpOwner->lcdFont;
    p.setPen(QPen(fontColor));
    n.setPointSize(lcdFontSize);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,tr("%1").arg(di,1,16));

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPDigit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change text color..."),this,SLOT(chooseColor()));
    cm.exec(event->globalPos());
}

void QCPDigit::chooseColor()
{
    QColor res = QColorDialog::getColor(fontColor,this);
    if (res.isValid()) {
        fontColor = res;
        update();
    }
}
