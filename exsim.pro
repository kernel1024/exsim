TEMPLATE      = app

TARGET        = exsim

INCLUDEPATH  += components

HEADERS       = components/cpbase.h \
                renderarea.h \
                mainwindow.h \
    components/cpbutton.h \
    components/cpled.h
SOURCES       = main.cpp \
                renderarea.cpp \
                mainwindow.cpp \
                components/cpbase.cpp \
    components/cpbutton.cpp \
    components/cpled.cpp

RESOURCES     = exsim.qrc

CONFIG += warn_on

FORMS += \
    mainwindow.ui
