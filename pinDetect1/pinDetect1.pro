TARGET = pinDetect1
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += pinDetect1_zh.ts \
    pinDetect1_en.ts \

OTHER_FILES += pinDetect1_zh.ts \
    pinDetect1_en.ts \
    pinDetect1.ini\
pinDetect1.ver\

HEADERS += pinDetect1Wdg.h \
    pinDetect1Mod.h \
    pinDetect1Fact.h \
    pinDetect1Param.h \

SOURCES += pinDetect1Wdg.cpp \
    pinDetect1Mod.cpp \
    pinDetect1Fact.cpp \
    pinDetect1Param.cpp \

FORMS += pinDetect1Wdg.ui

RESOURCES += pinDetect1.qrc

