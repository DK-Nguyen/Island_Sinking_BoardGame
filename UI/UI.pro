#-------------------------------------------------
#
# Project created by QtCreator 2018-07-12T13:53:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IslandGame
TEMPLATE = app
CONFIG += c++14

RESOURCES = assets.qrc

SUBDIRS += Images


SOURCES += main.cc \
    gameboard.cpp \
    gamestate.cpp \
    player.cpp \
    graphichex.cpp \
    graphicpawn.cpp \
    graphicactor.cpp \
    hexboard.cpp \
    mainwindow.cpp \
    controlboard.cpp \
    configurationwindow.cpp \
    wheel.cpp \
    graphictransport.cpp \
    gameover.cpp

HEADERS  += \
    gameboard.hh \
    player.hh \
    gamestate.hh \
    graphichex.h \
    graphicpawn.h \
    graphicactor.h \
    hexboard.h \
    controlboard.h \
    mainwindow.h \
    configurationwindow.h \
    wheel.h \
    graphictransport.h \
    gameover.h

INCLUDEPATH += $$PWD/../GameLogic/Engine
DEPENDPATH += $$PWD/../GameLogic/Engine

CONFIG(release, debug|release) {
   DESTDIR = release
}

CONFIG(debug, debug|release) {
   DESTDIR = debug
}

LIBS += -L$$OUT_PWD/../GameLogic/Engine
LIBS += -L$$OUT_PWD/../GameLogic/Engine/$${DESTDIR}/ -lEngine

 win32 {
    copyfiles.commands += @echo NOW COPYING ADDITIONAL FILE(S) for Windows &
    copyfiles.commands += @echo from \"$$_PRO_FILE_PWD_/../GameLogic/Assets\" to  \"$$OUT_PWD\" &
    copyfiles.commands += @call xcopy \"$$_PRO_FILE_PWD_/../GameLogic/Assets\" \"$$OUT_PWD//Assets\" /i /s /e /y
}
unix:!macx {
    copyfiles.commands += echo \"NOW COPYING ADDITIONAL FILE(S) for Linux\" &&
    copyfiles.commands += echo from $$_PRO_FILE_PWD_/../GameLogic/Assets to  $$OUT_PWD &
    copyfiles.commands += cp -r $$_PRO_FILE_PWD_/../GameLogic/Assets $$DESTDIR
}
 macx {
    copyfiles.commands += @echo \"NOW COPYING ADDITIONAL FILE(S) for MacOS\" &&
    copyfiles.commands += echo from $$_PRO_FILE_PWD_/../GameLogic/Assets to  $$OUT_PWD &
    copyfiles.commands += mkdir -p $$DESTDIR/IslandGame.app/Contents/MacOS &&
    copyfiles.commands += cp -r $$_PRO_FILE_PWD_/../GameLogic/Assets $$DESTDIR &&
    copyfiles.commands += cp -r $$_PRO_FILE_PWD_/../GameLogic/Assets $$DESTDIR/IslandGame.app/Contents/MacOS/
 }

QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles
