TARGET = standEdge
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += standEdge_zh.ts \
    standEdge_en.ts \

OTHER_FILES += standEdge_zh.ts \
    standEdge_en.ts \
    standEdge.ini\
standEdge.ver\

HEADERS += standEdgeWdg.h \
    standEdgeMod.h \
    standEdgeFact.h \
    standEdgeParam.h \

SOURCES += standEdgeWdg.cpp \
    standEdgeMod.cpp \
    standEdgeFact.cpp \
    standEdgeParam.cpp \

FORMS += standEdgeWdg.ui

RESOURCES += standEdge.qrc

