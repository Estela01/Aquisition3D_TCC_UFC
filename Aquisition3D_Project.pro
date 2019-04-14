#-------------------------------------------------
#
# Project created by QtCreator 2018-10-01T10:06:03
#
#-------------------------------------------------

QT       += core gui
QT += opengl


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aquisition3D_Project
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
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/boost/
linux-g++:QMAKE_CXXFLAGS +=  -march=native
linux-g++-64:QMAKE_CXXFLAGS +=  -march=native

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Qrealsense.cpp \
    QKinectSensor.cpp \
    rgbwindow.cpp \
    ofxkinectextras.cpp

HEADERS += \
        mainwindow.h \
    qrealsense.h \
    qkinectsensor.h \
    rgbwindow.h \
    stb_image_write.h \
    ofxkinectextras.h

INCLUDEPATH+=./include \
/usr/local/include/libfreenect \
/usr/local/include
INCLUDEPATH += /usr/local/include/librealsense2

INCLUDEPATH += /usr/include/pcl-1.8

LIBS += -L/usr/local/lib/ -lfreenect
LIBS += -lrealsense2



DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN


