TARGET = nccMatch_outN
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += nccMatch_outN_zh.ts \
    nccMatch_outN_en.ts \

OTHER_FILES += nccMatch_outN_zh.ts \
    nccMatch_outN_en.ts \
    nccMatch_outN.ini\
nccMatch_outN.ver\

HEADERS += nccMatch_outNWdg.h \
    nccMatch_outNMod.h \
    nccMatch_outNFact.h \
    nccMatch_outNParam.h \

SOURCES += nccMatch_outNWdg.cpp \
    nccMatch_outNMod.cpp \
    nccMatch_outNFact.cpp \
    nccMatch_outNParam.cpp \

FORMS += nccMatch_outNWdg.ui

RESOURCES += nccMatch_outN.qrc

