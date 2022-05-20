TARGET = pinDistance
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += pinDistance_zh.ts \
    pinDistance_en.ts \

OTHER_FILES += pinDistance_zh.ts \
    pinDistance_en.ts \
    pinDistance.ini\
pinDistance.ver\

HEADERS += pinDistanceWdg.h \
    pinDistanceMod.h \
    pinDistanceFact.h \
    pinDistanceParam.h \

SOURCES += pinDistanceWdg.cpp \
    pinDistanceMod.cpp \
    pinDistanceFact.cpp \
    pinDistanceParam.cpp \

FORMS += pinDistanceWdg.ui

RESOURCES += pinDistance.qrc

