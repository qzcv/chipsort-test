TARGET = edgeInter
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += edgeInter_zh.ts \
    edgeInter_en.ts \

OTHER_FILES += edgeInter_zh.ts \
    edgeInter_en.ts \
    edgeInter.ini\
edgeInter.ver\

HEADERS += edgeInterWdg.h \
    edgeInterMod.h \
    edgeInterFact.h \
    edgeInterParam.h \

SOURCES += edgeInterWdg.cpp \
    edgeInterMod.cpp \
    edgeInterFact.cpp \
    edgeInterParam.cpp \

FORMS += edgeInterWdg.ui

RESOURCES += edgeInter.qrc

