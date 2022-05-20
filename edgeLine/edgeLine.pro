TARGET = edgeLine
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += edgeLine_zh.ts \
    edgeLine_en.ts \

OTHER_FILES += edgeLine_zh.ts \
    edgeLine_en.ts \
    edgeLine.ini\
edgeLine.ver\

HEADERS += edgeLineWdg.h \
    edgeLineMod.h \
    edgeLineFact.h \
    edgeLineParam.h \

SOURCES += edgeLineWdg.cpp \
    edgeLineMod.cpp \
    edgeLineFact.cpp \
    edgeLineParam.cpp \

FORMS += edgeLineWdg.ui

RESOURCES += edgeLine.qrc

