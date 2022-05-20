TARGET = surfaceMult
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += surfaceMult_zh.ts \
    surfaceMult_en.ts \

OTHER_FILES += surfaceMult_zh.ts \
    surfaceMult_en.ts \
    surfaceMult.ini\
surfaceMult.ver\

HEADERS += surfaceMultWdg.h \
    surfaceMultMod.h \
    surfaceMultFact.h \
    surfaceMultParam.h \

SOURCES += surfaceMultWdg.cpp \
    surfaceMultMod.cpp \
    surfaceMultFact.cpp \
    surfaceMultParam.cpp \

FORMS += surfaceMultWdg.ui

RESOURCES += surfaceMult.qrc

