TARGET = edgeLocation
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += edgeLocation_zh.ts \
    edgeLocation_en.ts \

OTHER_FILES += edgeLocation_zh.ts \
    edgeLocation_en.ts \
    edgeLocation.ini\
edgeLocation.ver\

HEADERS += edgeLocationWdg.h \
    edgeLocationMod.h \
    edgeLocationFact.h \
    edgeLocationParam.h \

SOURCES += edgeLocationWdg.cpp \
    edgeLocationMod.cpp \
    edgeLocationFact.cpp \
    edgeLocationParam.cpp \

FORMS += edgeLocationWdg.ui

RESOURCES += edgeLocation.qrc

