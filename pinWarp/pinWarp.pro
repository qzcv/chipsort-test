TARGET = pinWarp
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += pinWarp_zh.ts \
    pinWarp_en.ts \

OTHER_FILES += pinWarp_zh.ts \
    pinWarp_en.ts \
    pinWarp.ini\
pinWarp.ver\

HEADERS += pinWarpWdg.h \
    pinWarpMod.h \
    pinWarpFact.h \
    pinWarpParam.h \

SOURCES += pinWarpWdg.cpp \
    pinWarpMod.cpp \
    pinWarpFact.cpp \
    pinWarpParam.cpp \

FORMS += pinWarpWdg.ui

RESOURCES += pinWarp.qrc

