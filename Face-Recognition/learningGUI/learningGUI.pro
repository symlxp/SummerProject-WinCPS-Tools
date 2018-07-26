#-------------------------------------------------
#
# Project created by QtCreator 2018-07-09T21:54:26
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = learningGUI
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    gmm.cpp \
    mfcc.cpp \
    voice.cpp

HEADERS += \
        mainwindow.h \
    gmm.h \
    mfcc.h \
    voice.h

FORMS += \
        mainwindow.ui \
    voice.ui
INCLUDEPATH += \
        D:/opencv/new_build/install/include\
        D:/opencv/new_build/install/include/opencv2\
        D:/opencv/new_build/install/include/opencv\
		D:/libfacedetection/include

LIBS += -LD:/opencv/new_build/install/x64/vc15/lib -lopencv_world341 -lopencv_img_hash341
LIBS += D:/libfacedetection/lib/libfacedetect-x64.lib
