#include "cplabel.h"

QCPLabel::QCPLabel(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    labelText = "text";
}

QSize QCPLabel::minimumSizeHint() const
{
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()+2)* zoom()/100);
    QFontMetrics fm(n);
    return QSize(fm.width(labelText),2*fm.height());
}

void QCPLabel::readFromStream(QDataStream &stream)
{
    stream >> labelText >> labelColor;
    QCPBase::readFromStream(stream);
    if (labelText.isEmpty()) checkRecycle(true);
}

void QCPLabel::storeToStream(QDataStream &stream)
{
    stream << labelText << labelColor;
    QCPBase::storeToStream(stream);
}

void QCPLabel::realignPins(QPainter &)
{
}

void QCPLabel::doLogicPrivate()
{
}

void QCPLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change text..."),this,SLOT(changeText()));
    cm.addAction(tr("Change text color..."),this,SLOT(changeColor()));
    cm.exec(event->globalPos());
}

void QCPLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
        changeText();
    else
        QCPBase::mouseDoubleClickEvent(event);
}

void QCPLabel::changeText()
{
    bool ok;
    QString s = QInputDialog::getText(this,tr("Text label"),tr("Text"),QLineEdit::Normal,labelText,&ok);
    if (ok && !s.isEmpty()) {
        labelText = s;
        resize(minimumSizeHint());
        update();
    }
}

void QCPLabel::changeColor()
{
    QColor s = QColorDialog::getColor(labelColor,this);
    if (s.isValid()) {
        labelColor = s;
        update();
    }
}

void QCPLabel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen op=p.pen();
    QFont of=p.font();

    QRect rc = rect();
    p.setPen(QPen(labelColor));
    QFont n = QApplication::font();
    n.setPointSize((n.pointSize()+2) * zoom()/100);
    p.setFont(n);
    p.drawText(rc,Qt::AlignCenter,labelText);

    p.setFont(of);
    p.setPen(op);
}