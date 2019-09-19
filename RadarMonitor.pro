#-------------------------------------------------
#
# Project created by QtCreator 2019-04-09T14:58:28
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadarMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    receivedata.cpp \
    getframesthread.cpp \
    radarunitdata.cpp \
    radarframeprocessthread.cpp \
    receivelicenseplate.cpp \
    getlicenseplatethread.cpp \
    licenseplateunit.cpp \
    radarradarcomparethread.cpp \
    licenseradarcomparethread.cpp \
    receivedatafromserver.cpp \
    RangeSlider.cpp \
    slider.cpp \
    sendconfig.cpp \
    filedownloader.cpp

HEADERS += \
        mainwindow.h \
    receivedata.h \
    getframesthread.h \
    radarunitdata.h \
    radarframeprocessthread.h \
    receivelicenseplate.h \
    getlicenseplatethread.h \
    licenseplateunit.h \
    radarradarcomparethread.h \
    licenseradarcomparethread.h \
    receivedatafromserver.h \
    RangeSlider.h \
    slider.h \
    sendconfig.h \
    filedownloader.h \
    basestruct.h

FORMS += \
        mainwindow.ui

LIBS += -lws2_32

QT += network

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



QMAKE_CXXFLAGS += -mno-ms-bitfields

