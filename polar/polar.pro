TARGET = polar
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += polar_zh.ts \
    polar_en.ts \

OTHER_FILES += polar_zh.ts \
    polar_en.ts \
    polar.ini\
polar.ver\

HEADERS += polarWdg.h \
    polarMod.h \
    polarFact.h \
    polarParam.h \

SOURCES += polarWdg.cpp \
    polarMod.cpp \
    polarFact.cpp \
    polarParam.cpp \

FORMS += polarWdg.ui

RESOURCES += polar.qrc

