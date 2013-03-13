#include "cpsynth.h"
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>

const float notes[32] = {130.81, 146.83, 164.81, 174.61, 196.00, 220.00, 246.94,
                         261.63, 293.67, 329.63, 349.23, 392.00, 440.00, 493.88,
                         523.25, 587.33, 659.26, 698.46, 783.99, 880.00, 987.77,
                         1046.5, 1174.7, 1318.5, 1396.9, 1568.0, 1760.0, 1975.5,
                         2093.0, 2349.3, 2637.0, 2793.8};

bool al_check_error_synth(QString where) {
    ALenum err = alGetError();
    if (err==AL_NO_ERROR) return true;
    for(; err!=AL_NO_ERROR; err=alGetError())
        qDebug() << "AL Error " << QString("%1").arg(err,0,16) << " at " << where;
    return false;
}

void drawRotatedText(QPainter *painter, float degrees, const QRect& rect, const QString &text)
{
    painter->save();
    painter->translate(rect.center());
    painter->rotate(degrees);
    QRect rct = QRect(-rect.height()/2,-rect.width()/2,rect.height(),rect.width());
    painter->drawText(rct, Qt::AlignCenter, text);
    painter->restore();
}

QCPSynth::QCPSynth(QWidget *parent, QRenderArea *aOwner) :
    QCPBase(parent,aOwner)
{
    f1Inp = new QCPInput(this,this,"1"); fInputs.append(f1Inp);
    f2Inp = new QCPInput(this,this,"2"); fInputs.append(f2Inp);
    f4Inp = new QCPInput(this,this,"4"); fInputs.append(f4Inp);
    f8Inp = new QCPInput(this,this,"8"); fInputs.append(f8Inp);
    f16Inp = new QCPInput(this,this,"16"); fInputs.append(f16Inp);
    fOEInp = new QCPInput(this,this,"OE"); fInputs.append(fOEInp);

    alError=false;
    alPlaying=false;
    albuf=0;
    alsrc=0;
    updateFreq(28);
}

QCPSynth::~QCPSynth()
{
    if (alPlaying) {
        alSourceStop(alsrc);
        if (!al_check_error_synth("alSourceStop"))
            alError=true;
        alDeleteSources(1,&alsrc);
        if (!al_check_error_synth("alDeleteSources"))
            alError=true;
        alsrc=0;
    }
    fInputs.clear();
    delete f1Inp;
    delete f2Inp;
    delete f4Inp;
    delete f8Inp;
    delete f16Inp;
    delete fOEInp;
}

void QCPSynth::updateFreq(int code)
{
    if ((code<0) || (code>31)) return;
    if (alPlaying) {
        alSourceStop(alsrc);
        if (!al_check_error_synth("alSourceStop"))
            alError=true;
        alDeleteSources(1,&alsrc);
        if (!al_check_error_synth("alDeleteSources"))
            alError=true;
        alsrc=0;
    }

    freq = notes[code];
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
    if (!al_check_error_synth("alGenSources"))
        alError=true;
    else {
        alBufferData(albuf, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
        if (!al_check_error_synth("alBufferData"))
            alError=true;
    }
    if (alPlaying) {
        alGenSources(1, &alsrc);
        if (!al_check_error_synth("alGenSources"))
            alError=true;
        alSourcei(alsrc, AL_BUFFER, albuf);
        if (!al_check_error_synth("alSourcei"))
            alError=true;
        alSourcei(alsrc, AL_LOOPING, AL_TRUE);
        if (!al_check_error_synth("alSourcei loop"))
            alError=true;
        alSourcePlay(alsrc);
        if (!al_check_error_synth("alSourcePlay"))
            alError=true;
    }
    savedCode = code;
}

QSize QCPSynth::minimumSizeHint() const
{
    return QSize(80*zoom()/100,getDCompHeight(1));
}

void QCPSynth::realignPins(QPainter &)
{
    int dy = getDCompIncrement();
    int ddy = dy/2;
    f1Inp->relCoord=QPoint(getPinSize()/2,dy);
    f2Inp->relCoord=QPoint(getPinSize()/2,2*dy);
    f4Inp->relCoord=QPoint(getPinSize()/2,3*dy);
    f8Inp->relCoord=QPoint(getPinSize()/2,4*dy);
    f16Inp->relCoord=QPoint(getPinSize()/2,5*dy);
    fOEInp->relCoord=QPoint(getPinSize()/2,6*dy+ddy);
}

void QCPSynth::doLogicPrivate()
{
}

void QCPSynth::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPen op=p.pen();
    QBrush ob=p.brush();
    QFont of=p.font();

    qint8 di = ((f16Inp->state << 4) & 0x10) |
               ((f8Inp->state << 3) & 0x8) |
               ((f4Inp->state << 2) & 0x4) |
               ((f2Inp->state << 1) & 0x2) |
               (f1Inp->state & 0x1);

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

    int dy = getDCompIncrement();
    int ddy = dy/2;

    rc = rect();
    rc.adjust(p.fontMetrics().width("OE")+getPinSize()/2,0,-1,-1);
    p.drawLine(rc.topLeft(),rc.bottomLeft());

    p.drawLine(0,5*dy+ddy+getPinSize()/2,rc.left(),5*dy+ddy+getPinSize()/2);

    n.setBold(true);
    p.setFont(n);
    drawRotatedText(&p,90,rc,"Synth");

    p.setFont(of);
    p.setBrush(ob);
    p.setPen(op);

    drawSelection(p);

    if (!alPlaying && fOEInp->state) {
        alsrc = 0;
        alGenSources(1, &alsrc);
        alSourcei(alsrc, AL_BUFFER, albuf);
        alSourcei(alsrc, AL_LOOPING, AL_TRUE);
        alSourcePlay(alsrc);
        if (!al_check_error_synth("alSourcePlay paint"))
            alError=true;
        alPlaying=true;
    } else if (alPlaying && !fOEInp->state) {
        alSourceStop(alsrc);
        if (!al_check_error_synth("alSourceStop paint"))
            alError=true;
        alPlaying=false;
    } else if (di!=savedCode) {
        updateFreq(di);
    }
}
