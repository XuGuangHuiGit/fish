#-------------------------------------------------
#
# Project created by QtCreator 2016-09-27T09:10:18
#
#-------------------------------------------------

QT       += core gui

QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pwm
TEMPLATE = app


SOURCES += main.cpp\
        serial.cpp

HEADERS  += serial.h

FORMS    += \
    serial.ui


