#include "cpram.h"
#include "renderarea.h"

int ipow(int base, int pow)
{
    qlonglong t = base;
    if (pow == 0) t = 1;
    for (int i=1;i<pow;i++)
        t = t*base;
    return t;
}

QCPRAM::QCPRAM(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    addrWidth = 10;
    savedW = false;

    fD0Out = new QCPOutput(this,this,"Q0"); fOutputs.append(fD0Out);
    fD1Out = new QCPOutput(this,this,"Q1"); fOutputs.append(fD1Out);
    fD2Out = new QCPOutput(this,this,"Q2"); fOutputs.append(fD2Out);
    fD3Out = new QCPOutput(this,this,"Q3"); fOutputs.append(fD3Out);
    fD4Out = new QCPOutput(this,this,"Q4"); fOutputs.append(fD4Out);
    fD5Out = new QCPOutput(this,this,"Q5"); fOutputs.append(fD5Out);
    fD6Out = new QCPOutput(this,this,"Q6"); fOutputs.append(fD6Out);
    fD7Out = new QCPOutput(this,this,"Q7"); fOutputs.append(fD7Out);

    updateInputsCount();
}

QCPRAM::~QCPRAM()
{
    while (!fInputs.isEmpty()) {
        QCPInput* inp = fInputs.takeFirst();
        delete inp;
    }
    while (!fOutputs.isEmpty()) {
        QCPOutput* out = fOutputs.takeFirst();
        delete out;
    }
}

void QCPRAM::updateInputsCount()
{
    // clear and resize data
    data.clear();
    data.fill('\x00',ipow(2,addrWidth));

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
    for(int i=0;i<addrWidth;i++)
        fInputs << new QCPInput(this,this,tr("A%1").arg(i));

    fD0Inp = new QCPInput(this,this,"D0"); fInputs.append(fD0Inp);
    fD1Inp = new QCPInput(this,this,"D1"); fInputs.append(fD1Inp);
    fD2Inp = new QCPInput(this,this,"D2"); fInputs.append(fD2Inp);
    fD3Inp = new QCPInput(this,this,"D3"); fInputs.append(fD3Inp);
    fD4Inp = new QCPInput(this,this,"D4"); fInputs.append(fD4Inp);
    fD5Inp = new QCPInput(this,this,"D5"); fInputs.append(fD5Inp);
    fD6Inp = new QCPInput(this,this,"D6"); fInputs.append(fD6Inp);
    fD7Inp = new QCPInput(this,this,"D7"); fInputs.append(fD7Inp);

    fWInp = new QCPInput(this,this,"WR"); fInputs.append(fWInp);
    fOEInp = new QCPInput(this,this,"OE"); fInputs.append(fOEInp);

    cpOwner->repaintConn();
    resize(minimumSizeHint());
    update();
}

QSize QCPRAM::minimumSizeHint() const
{
    return QSize(100*zoom()/100,getDCompHeight(2));
}

void QCPRAM::readFromStream(QDataStream &stream)
{
    stream >> addrWidth;
    updateInputsCount();
    stream >> data;
    QCPBase::readFromStream(stream);
}

void QCPRAM::storeToStream(QDataStream &stream)
{
    stream << addrWidth;
    stream << data;
    QCPBase::storeToStream(stream);
}

void QCPRAM::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    fD0Out->relCoord = QPoint(width()-getPinSize()/2,dy);
    fD1Out->relCoord = QPoint(width()-getPinSize()/2,2*dy);
    fD2Out->relCoord = QPoint(width()-getPinSize()/2,3*dy);
    fD3Out->relCoord = QPoint(width()-getPinSize()/2,4*dy);
    fD4Out->relCoord = QPoint(width()-getPinSize()/2,5*dy);
    fD5Out->relCoord = QPoint(width()-getPinSize()/2,6*dy);
    fD6Out->relCoord = QPoint(width()-getPinSize()/2,7*dy);
    fD7Out->relCoord = QPoint(width()-getPinSize()/2,8*dy);
    for(int i=0;i<addrWidth;i++)
        fInputs[i]->relCoord = QPoint(getPinSize()/2,(i+1)*dy);
    for(int i=0;i<8;i++)
        fInputs[i+addrWidth]->relCoord = QPoint(getPinSize()/2,(i+addrWidth+1)*dy+ddy);
    fWInp->relCoord = QPoint(getPinSize()/2,(addrWidth+9)*dy+2*ddy);
    fOEInp->relCoord = QPoint(getPinSize()/2,(addrWidth+10)*dy+2*ddy);
}

void QCPRAM::doLogicPrivate()
{
    bool W = fWInp->state;
    bool OE = fOEInp->state;
    qint8 di = 0;
    int ai = 0;
    for(int i=0;i<addrWidth;i++)
        ai = ai | ((fInputs[i]->state & 0x1) << i);
    for(int i=0;i<8;i++)
        di = di | ((fInputs[addrWidth+i]->state & 0x1) << i);

    if (!W && savedW && (ai>=0) && (ai<data.size()))
        data[ai]=di;
    di = 0;
    if (OE && (ai>=0) && (ai<data.size()))
        di = data[ai];

    savedW = W;
    for(int i=0;i<8;i++)
        fOutputs[i]->state = ((di & ipow(2,i)) > 0);
}

void QCPRAM::paintEvent(QPaintEvent *)
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

    QFont n=QApplication::font();
    p.setPen(QPen(Qt::black));
    n.setPointSize((n.pointSize()+4)*zoom()/100);
    n.setBold(true);
    p.setFont(n);

    rc = rect();
    rc.adjust(p.fontMetrics().width("OE")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    int dy = getDCompIncrement();
    int ddy = dy/2;
    p.drawLine(0,addrWidth*dy+ddy+getPinSize()/2,rc.left(),addrWidth*dy+ddy+getPinSize()/2);
    p.drawLine(0,(addrWidth+8)*dy+2*ddy+getPinSize()/2,rc.left(),(addrWidth+8)*dy+2*ddy+getPinSize()/2);

    rc = rect();
    rc.adjust(width()-p.fontMetrics().width("OE")-getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    rc = rect();
    rc.adjust(0,0,-1,0);
    rc.setHeight(getDCompIncrement()*3);
    n.setBold(false);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,"RAM");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);
}

void QCPRAM::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Load data from file..."),this,SLOT(loadFromFile()));
    cm.addAction(tr("Save data to file..."),this,SLOT(saveToFile()));
    cm.addSeparator();
    cm.addAction(tr("Change RAM size..."),this,SLOT(changeSize()));
    cm.addAction(tr("Reset data"),this,SLOT(resetData()));
    cm.exec(event->globalPos());

}

void QCPRAM::loadFromFile()
{
    QString fname = QFileDialog::getOpenFileName(this,tr("exSim"));
    if (fname.isEmpty()) return;
    QFile f(fname);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,tr("exSim error"),tr("Cannot open binary file"));
        return;
    }
    resetData();
    QByteArray b = f.read(data.size());
    data.replace(0,b.size(),b);
    f.close();
    b.clear();
}

void QCPRAM::saveToFile()
{
    QString fname = QFileDialog::getSaveFileName(this,tr("exSim"));
    if (fname.isEmpty()) return;
    QFile f(fname);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,tr("exSim error"),tr("Cannot create binary file"));
        return;
    }
    f.write(data);
    f.close();
}

void QCPRAM::changeSize()
{
    bool ok;
    int tmp = QInputDialog::getInt(this,tr("exSim"),tr("RAM address bus width"),addrWidth,1,20,1,&ok);
    if (ok) {
        addrWidth = tmp;
        updateInputsCount();
    }
}

void QCPRAM::resetData()
{
    data.fill('\x00');
}
