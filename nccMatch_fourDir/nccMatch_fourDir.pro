TARGET = nccMatch_fourDir
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += nccMatch_fourDir_zh.ts \
    nccMatch_fourDir_en.ts \

OTHER_FILES += nccMatch_fourDir_zh.ts \
    nccMatch_fourDir_en.ts \
    nccMatch_fourDir.ini\
nccMatch_fourDir.ver\

HEADERS += nccMatch_fourDirWdg.h \
    nccMatch_fourDirMod.h \
    nccMatch_fourDirFact.h \
    nccMatch_fourDirParam.h \

SOURCES += nccMatch_fourDirWdg.cpp \
    nccMatch_fourDirMod.cpp \
    nccMatch_fourDirFact.cpp \
    nccMatch_fourDirParam.cpp \

FORMS += nccMatch_fourDirWdg.ui

RESOURCES += nccMatch_fourDir.qrc

