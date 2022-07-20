TARGET = standLocationNew
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += standLocationNew_zh.ts \
    standLocationNew_en.ts \

OTHER_FILES += standLocationNew_zh.ts \
    standLocationNew_en.ts \
    standLocationNew.ini\
standLocationNew.ver\

HEADERS += standLocationNewWdg.h \
    standLocationNewMod.h \
    standLocationNewFact.h \
    standLocationNewParam.h \

SOURCES += standLocationNewWdg.cpp \
    standLocationNewMod.cpp \
    standLocationNewFact.cpp \
    standLocationNewParam.cpp \

FORMS += standLocationNewWdg.ui

RESOURCES += standLocationNew.qrc

