TARGET = location
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += location_zh.ts \
    location_en.ts \

OTHER_FILES += location_zh.ts \
    location_en.ts \
    location.ini\
location.ver\

HEADERS += locationWdg.h \
    locationMod.h \
    locationFact.h \
    locationParam.h \

SOURCES += locationWdg.cpp \
    locationMod.cpp \
    locationFact.cpp \
    locationParam.cpp \

FORMS += locationWdg.ui

RESOURCES += location.qrc

