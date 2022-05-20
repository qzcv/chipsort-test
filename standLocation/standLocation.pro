TARGET = standLocation
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += standLocation_zh.ts \
    standLocation_en.ts \

OTHER_FILES += standLocation_zh.ts \
    standLocation_en.ts \
    standLocation.ini\
standLocation.ver\

HEADERS += standLocationWdg.h \
    standLocationMod.h \
    standLocationFact.h \
    standLocationParam.h \

SOURCES += standLocationWdg.cpp \
    standLocationMod.cpp \
    standLocationFact.cpp \
    standLocationParam.cpp \

FORMS += standLocationWdg.ui

RESOURCES += standLocation.qrc

