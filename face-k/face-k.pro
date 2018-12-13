#-------------------------------------------------
#
# Project created by QtCreator 2018-03-05T10:39:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = face-k
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    inc/Kinect.h \
    inc/Kinect.INPC.h

FORMS    += mainwindow.ui

# Kinect
win32: LIBS += -L$$PWD/inc/ -lKinect20
INCLUDEPATH += $$PWD/inc
DEPENDPATH += $$PWD/inc

# OpenCV
INCLUDEPATH += C:\opencv\build\include
# Mettre le chemin vers l'install d'OpenCV et ajouté les bonnes libs
# Normalement, seul core, highgui et imgproc sont utilisés dans l'appli
LIBS += C:\opencv-build\bin\libopencv_core344.dll
LIBS += C:\opencv-build\bin\libopencv_highgui344.dll
LIBS += C:\opencv-build\bin\libopencv_imgproc344.dll
LIBS += C:\opencv-build\bin\libopencv_features2d344.dll
LIBS += C:\opencv-build\bin\libopencv_calib3d344.dll
