#-------------------------------------------------
#
# Project created by QtCreator 2018-09-17T16:03:00
#
#-------------------------------------------------

QT       += core gui opengl
LIBS     += -lGL

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ARM
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vertices.cpp

HEADERS  += mainwindow.h \
    vertices.h
