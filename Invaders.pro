QT -= gui
TARGET = Invaders
DESTDIR = ./
SOURCES += src/*.c
HEADERS += include/*.h
cache()
BASE_DIR = ./
QMAKE_CFLAGS += -std=c99 -Wall -g
QMAKE_CFLAGS += $$system(sdl2-config  --cflags)
message(output from sdl2-config --cflags added to CXXFLAGS= $$QMAKE_CFLAGS)
INCLUDEPATH += /usr/local/include/SDL2/
INCLUDEPATH += $$BASE_DIR/include/
LIBS += $$system(sdl2-config  --libs)
message(output from sdl2-config --libs added to LIB=$$LIBS)
LIBS += -lSDL2_image -lSDL2_mixer -lSDL2_ttf
macx:DEFINES += MAC_OS_X_VERSION_MIN_REQUIRED=1060
CONFIG += console
CONFIG -= app_bundle
