#-------------------------------------------------
#
# Project created by QtCreator 2014-10-16T17:16:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RJManager
TEMPLATE = app
QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES += main.cpp\
        rjmanager.cpp

HEADERS  += rjmanager.h

FORMS    += rjmanager.ui

LIBS += -L$$PWD/curl/lib/ -lcurl
LIBS += -L$$PWD/curl/lib/ -lcurldll

INCLUDEPATH += $$PWD/curl/include
