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
    components/cpbeeper.h
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
    components/cpbeeper.cpp

RESOURCES     = exsim.qrc

CONFIG += warn_on link_pkgconfig

LIBS += -lopenal

FORMS += \
    mainwindow.ui
