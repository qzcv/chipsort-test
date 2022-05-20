TARGET = edgeWarp
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += edgeWarp_zh.ts \
    edgeWarp_en.ts \

OTHER_FILES += edgeWarp_zh.ts \
    edgeWarp_en.ts \
    edgeWarp.ini\
edgeWarp.ver\

HEADERS += edgeWarpWdg.h \
    edgeWarpMod.h \
    edgeWarpFact.h \
    edgeWarpParam.h \

SOURCES += edgeWarpWdg.cpp \
    edgeWarpMod.cpp \
    edgeWarpFact.cpp \
    edgeWarpParam.cpp \

FORMS += edgeWarpWdg.ui

RESOURCES += edgeWarp.qrc

