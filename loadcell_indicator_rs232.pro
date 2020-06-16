QT += core gui widgets serialport printsupport

CONFIG += c++11

SOURCES += \
    logger.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    serialcomm.cpp

HEADERS += \
    logger.h \
    mainwindow.h \
    qcustomplot.h \
    serialcomm.h

FORMS += \
    mainwindow.ui
