TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
LIBS+= -lfuse

HEADERS += \
    json.hpp \
    ccfw.h