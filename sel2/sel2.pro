TARGET = sel2
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += sel2_zh.ts \
    sel2_en.ts \

OTHER_FILES += sel2_zh.ts \
    sel2_en.ts \
    sel2.ini\
sel2.ver\

HEADERS += sel2Wdg.h \
    sel2Mod.h \
    sel2Fact.h \
    sel2Param.h \

SOURCES += sel2Wdg.cpp \
    sel2Mod.cpp \
    sel2Fact.cpp \
    sel2Param.cpp \

FORMS += sel2Wdg.ui

RESOURCES += sel2.qrc

