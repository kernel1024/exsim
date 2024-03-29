QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE      = app

TARGET        = exsim

INCLUDEPATH  += components

HEADERS       = components/cpbase.h \
                renderarea.h \
                mainwindow.h \
    components/cpbutton.h \
    components/cpled.h \
    components/cplogic.h \
    components/cpextender.h \
    components/cptrigger.h \
    components/cpgen.h \
    components/cpbeeper.h \
    components/cpregister.h \
    components/cpdecoder.h \
    components/cpencoder.h \
    components/cpcounter.h \
    components/cpdigit.h \
    components/cpmux.h \
    components/cpsum.h \
    components/cpgnd.h \
    components/cpvcc.h \
    components/cpcomparator.h \
    components/cprndgen.h \
    components/cpram.h \
    components/cpsynth.h \
    components/cplabel.h \
    loadinglogdialog.h \
    components/cpintextender.h \
    openal.h
SOURCES       = main.cpp \
                renderarea.cpp \
                mainwindow.cpp \
                components/cpbase.cpp \
    components/cpbutton.cpp \
    components/cpled.cpp \
    components/cplogic.cpp \
    components/cpextender.cpp \
    components/cptrigger.cpp \
    components/cpgen.cpp \
    components/cpbeeper.cpp \
    components/cpregister.cpp \
    components/cpdecoder.cpp \
    components/cpencoder.cpp \
    components/cpcounter.cpp \
    components/cpdigit.cpp \
    components/cpmux.cpp \
    components/cpsum.cpp \
    components/cpgnd.cpp \
    components/cpvcc.cpp \
    components/cpcomparator.cpp \
    components/cprndgen.cpp \
    components/cpram.cpp \
    components/cpsynth.cpp \
    components/cplabel.cpp \
    loadinglogdialog.cpp \
    components/cpintextender.cpp

RESOURCES     = exsim.qrc

CONFIG += warn_on

unix {
    CONFIG += link_pkgconfig
    LIBS += -lopenal
}

win32 {
    win32-g++:contains(QMAKE_HOST.arch, x86_64):{
        LIBS += $$PWD/lib64/libOpenAl32.dll.a
    } else {
        LIBS += $$PWD/lib32/libOpenAl32.dll.a
    }
}

FORMS += \
    mainwindow.ui \
    loadinglogdialog.ui
