QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app


TARGET = tst_gamestatetest

SOURCES +=  \
            tst_gamestatetest.cpp \
            ../../../UI/gamestate.cpp \
            ../../../UI/player.cpp \
            ../../../GameLogic/Engine/gameexception.cpp \


DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
        ../../../UI/gamestate.hh \
        ../../../UI/player.hh \
        ../../../GameLogic/Engine/igamestate.hh \

INCLUDEPATH += \
            ../../../UI \
            ../../../GameLogic/Engine \

DEPENDPATH += ../../../UI


