#include "cpbeeper.h"
#include <math.h>
#include <QInputDialog>

bool al_check_error(QString where) {
    ALenum err = alGetError();
    if (err==AL_NO_ERROR) return true;
    for(; err!=AL_NO_ERROR; err=alGetError())
        qDebug() << "AL Error " << QString("%1").arg(err,0,16) << " at " << where;
    return false;
}

QCPBeeper::QCPBeeper(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    fInp = new QCPInput(this,this);
    fInputs.append(fInp);

    alError=false;
    alPlaying=false;
    albuf=0;
    alsrc=0;
    updateFreq(2000.0);
}

QCPBeeper::~QCPBeeper()
{
    if (isSoundOK()) {
        if (alPlaying) {
            alSourceStop(alsrc);
            if (!al_check_error("alSourceStop"))
                alError=true;
            alDeleteSources(1,&alsrc);
            if (!al_check_error("alDeleteSources"))
                alError=true;
            alsrc=0;
        }
    }
    fInputs.clear();
    delete fInp;
}

void QCPBeeper::updateFreq(float aFreq)
{
    if (!stateUpdate.tryLock()) {
        qDebug() << "State update mutex is locked. Unable to change frequency.";
        return;
    }
    freq = aFreq;

    if (isSoundOK()) {
        if (alPlaying) {
            alSourceStop(alsrc);
            if (!al_check_error("alSourceStop"))
                alError=true;
            alDeleteSources(1,&alsrc);
            if (!al_check_error("alDeleteSources"))
                alError=true;
            alsrc=0;
        }

        int seconds = 4;
        unsigned sample_rate = 22050;
        size_t buf_size = seconds * sample_rate;

        short *samples;
        samples = new short[buf_size];
        for(size_t i=0; i<buf_size; ++i) {
            samples[i] = 32760 * sin( (2.f*float(M_PI)*freq)/sample_rate * i );
        }

        /* Download buffer to OpenAL */
        if (albuf!=0)
            alDeleteBuffers(1,&albuf);
        alGetError();
        albuf=0;
        alGenBuffers(1, &albuf);
        if (!al_check_error("alGenSources"))
            alError=true;
        else {
            alBufferData(albuf, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
            if (!al_check_error("alBufferData"))
                alError=true;
        }
        if (alPlaying) {
            alGenSources(1, &alsrc);
            if (!al_check_error("alGenSources"))
                alError=true;
            alSourcei(alsrc, AL_BUFFER, albuf);
            if (!al_check_error("alSourcei"))
                alError=true;
            alSourcei(alsrc, AL_LOOPING, AL_TRUE);
            if (!al_check_error("alSourcei loop"))
                alError=true;
            alSourcePlay(alsrc);
            if (!al_check_error("alSourcePlay"))
                alError=true;
        }
    }
    stateUpdate.unlock();

    update();
}

QSize QCPBeeper::minimumSizeHint() const
{
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    QFontMetrics fm(n);
    return QSize(fm.width(" SND: xxx Hz Q")+2*getPinSize()*zoom()/100,
                 2*fm.height());
}

void QCPBeeper::readFromXML(QTextStream &errlog, const QDomElement &element)
{
    bool ok;
    freq = element.attribute("freq","2000.0").toFloat(&ok);
    if ((!ok) || (freq<20.0) || (freq>20000.0)) {
        errlog << tr("QCPBeeper: freq value incorrect") << endl;
        freq = 2000.0;
    }
    updateFreq(freq);
    QCPBase::readFromXML(errlog,element);
}

void QCPBeeper::storeToXML(QDomElement &element)
{
    element.setAttribute("freq",tr("%1").arg(freq,1,'f',1));
    QCPBase::storeToXML(element);
}

void QCPBeeper::realignPins()
{
    fInp->relCoord=QPoint(getPinSize()/2,height()/2);
}

void QCPBeeper::doLogicPrivate()
{
    periodicCheck();
}

void QCPBeeper::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu cm(this);
    cm.addAction(tr("Change frequency..."),this,SLOT(chooseFreq()));
    cm.exec(event->globalPos());
}

bool QCPBeeper::checkTimerNeeded()
{
    return true;
}

void QCPBeeper::periodicCheck()
{
    if (!stateUpdate.tryLock()) return;
    if (isSoundOK()) {
        if (!alPlaying && fInp->state) {
            alsrc = 0;
            alGenSources(1, &alsrc);
            alSourcei(alsrc, AL_BUFFER, albuf);
            alSourcei(alsrc, AL_LOOPING, AL_TRUE);
            alSourcePlay(alsrc);
            if (!al_check_error("alSourcePlay paint"))
                alError=true;
            alPlaying=true;
        } else if (alPlaying && !fInp->state) {
            alSourceStop(alsrc);
            if (!al_check_error("alSourceStop paint"))
                alError=true;
            alPlaying=false;
        }
    }
    stateUpdate.unlock();
}

void QCPBeeper::chooseFreq()
{
    bool ok;
    double cnt = QInputDialog::getDouble(this,tr("Beeper frequency"),tr("Frequency (Hz)"),freq,20,20000,1,&ok);
    if (ok)
        updateFreq(cnt);
}

void QCPBeeper::paintEvent(QPaintEvent *)
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

    rc.adjust(getPinSize(),0,0,0);
    QFont n=QApplication::font();
    n.setPointSize((n.pointSize()) * zoom()/100);
    p.setFont(n);
    if (freq>=1000.0)
        p.drawText(rc,Qt::AlignCenter,tr("SND: %1 kHz").arg(freq/1000,1,'f',1));
    else
        p.drawText(rc,Qt::AlignCenter,tr("SND: %1 Hz").arg(freq));

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);
}
