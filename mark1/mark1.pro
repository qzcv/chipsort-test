TARGET = mark1
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += mark1_zh.ts \
    mark1_en.ts \

OTHER_FILES += mark1_zh.ts \
    mark1_en.ts \
    mark1.ini\
mark1.ver\

HEADERS += mark1Wdg.h \
    mark1Mod.h \
    mark1Fact.h \
    mark1Param.h \

SOURCES += mark1Wdg.cpp \
    mark1Mod.cpp \
    mark1Fact.cpp \
    mark1Param.cpp \

FORMS += mark1Wdg.ui

RESOURCES += mark1.qrc

