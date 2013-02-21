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
    components/cpram.h
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
    components/cpram.cpp

RESOURCES     = exsim.qrc

CONFIG += warn_on link_pkgconfig

LIBS += -lopenal

FORMS += \
    mainwindow.ui
