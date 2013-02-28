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
    calcDigitSize();
}

QCPDigit::~QCPDigit()
{
    fInputs.clear();
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete f8Inp;
}

void QCPDigit::calcDigitSize()
{
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

void QCPDigit::zoomChanged()
{
    QCPBase::zoomChanged();
    calcDigitSize();
    update();
}

QSize QCPDigit::minimumSizeHint() const
{
    return QSize(80*zoom()/100, getDCompHeight(0));
}

void QCPDigit::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    fontColor = QColor(element.attribute("fontColor","green"));
    if (!fontColor.isValid()) {
        errlog << tr("QCPDigit: fontColor has unknown name") << endl;
        fontColor = QColor(Qt::green);
    }
    QCPBase::readFromXML(errlog,element);
}

void QCPDigit::storeToXML(QDomElement &element)
{
    element.setAttribute("fontColor",fontColor.name());
    QCPBase::storeToXML(element);
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

    QFont n=QApplication::font();
    p.setPen(QPen(Qt::black));
    n.setPointSize((n.pointSize()+4)*zoom()/100);
    n.setBold(true);
    p.setFont(n);

    rc = rect();
    rc.adjust(p.fontMetrics().width("8")+getPinSize(),0,-1,-1);
    p.setBrush(QBrush(Qt::black,Qt::SolidPattern));
    p.drawRect(rc);

    n = cpOwner->lcdFont;
    p.setPen(QPen(fontColor));
    n.setPointSize(lcdFontSize);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,tr("%1").arg(di,1,16));

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
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
