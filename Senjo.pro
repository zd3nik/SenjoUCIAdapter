TEMPLATE = lib
VERSION = 1.0
CONFIG += console
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    src/BackgroundCommand.h \
    src/ChessEngine.h \
    src/ChessMove.h \
    src/EngineOption.h \
    src/MoveFinder.h \
    src/Output.h \
    src/Platform.h \
    src/Square.h \
    src/Threading.h \
    src/UCIAdapter.h

SOURCES += \
    src/BackgroundCommand.cpp \
    src/ChessEngine.cpp \
    src/EngineOption.cpp \
    src/MoveFinder.cpp \
    src/Output.cpp \
    src/Threading.cpp \
    src/UCIAdapter.cpp

OTHER_FILES += \
    epd/*.epd \
    README.md \
    uci-spec.txt
