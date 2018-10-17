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


SOURCES += \
    main.cpp \
    openglwindow.cpp \
    displayWindow.cpp \
    tinyobjloader/tiny_obj_loader.cc \
    obj.cpp \
    pgm3d.cpp

HEADERS  += \
    openglwindow.h\
    displayWindow.h \
    tinyobjloader/tiny_obj_loader.h \
    obj.h \
    pgm3d.h
