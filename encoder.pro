QT       += core gui charts datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    checkdatathread.cpp \
    conform.cpp \
    displaychart.cpp \
    fdctref.cpp \
    display.cpp \
    gettimeofday.cpp \
    idct.cpp \
    main.cpp \
    motion.cpp \
    mpeg2enc.cpp \
    predict.cpp \
    putbits.cpp \
    puthdr.cpp \
    putmpg.cpp \
    putpic.cpp \
    putseq.cpp \
    putvlc.cpp \
    quantize.cpp \
    ratectl.cpp \
    readpic.cpp \
    refreshbar.cpp \
    refreshui.cpp \
    scatterdatamodifier.cpp \
    signalshooter.cpp \
    stats.cpp \
    transfrm.cpp \
    visual3d.cpp \
    writepic.cpp

HEADERS += \
    checkdatathread.h \
    config.h \
    display.h \
    displaychart.h \
    gettimeofday.h \
    global.h \
    mpeg2enc.h \
    refreshbar.h \
    refreshui.h \
    scatterdatamodifier.h \
    signalshooter.h \
    visual3d.h \
    vlc.h

FORMS += \
    display.ui

TRANSLATIONS += \
    encoder_zh_CN.ts

CONFIG+= console

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
