TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt

SOURCES += \
        main.cpp \
    date.cc \
    fraction.cc \
    string.cc \
    vector.cc

DISTFILES += \
    Doxyfile

HEADERS += \
    date.hh \
    fraction.hh \
    string.hh \
    vector.hh

