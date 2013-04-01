#include "cplabel.h"
#include <QInputDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QLineEdit>

QCPLabel::QCPLabel(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    labelText = "text";
    labelColor = QColor(Qt::black);
    labelFont = QApplication::font();
    labelFont.setPointSize((labelFont.pointSize()+2) * zoom()/100);
}

QSize QCPLabel::minimumSizeHint() const
{
    QFontMetrics fm(labelFont);
    return QSize(fm.width(labelText),2*fm.height());
}

void QCPLabel::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    labelText = element.attribute("labelText","text");
    labelColor = element.attribute("labelColor","black");
    if (!labelColor.isValid()) {
        errlog << tr("QCPLabel: incorrect labelColor value") << endl;
        labelColor = QColor(Qt::black);
    }
    if (!labelFont.fromString(element.attribute("labelFont",""))) {
        errlog << tr("QCPLabel: incorrect labelFont value") << endl;
        labelFont = QApplication::font();
        labelFont.setPointSize((labelFont.pointSize()+2)*zoom()/100);
    }
    QCPBase::readFromXML(errlog,element);
    if (labelText.isEmpty()) deleteComponent();
}

void QCPLabel::storeToXML(QDomElement &element)
{
    element.setAttribute("labelText",labelText);
    element.setAttribute("labelColor",labelColor.name());
    element.setAttribute("labelFont",labelFont.toString());
    QCPBase::storeToXML(element);
}

void QCPLabel::realignPins()
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
    cm.addAction(tr("Change font..."),this,SLOT(changeFont()));
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

void QCPLabel::changeFont()
{
    bool ok;
    QFont f = QFontDialog::getFont(&ok,labelFont,this,tr("Text label font"));
    if (ok) {
        labelFont = f;
        resize(minimumSizeHint());
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
    p.setFont(labelFont);
    p.drawText(rc,Qt::AlignCenter,labelText);

    p.setFont(of);
    p.setPen(op);

    drawSelection(p);
}
