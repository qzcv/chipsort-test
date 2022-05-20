TARGET = pinLength1
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += pinLength1_zh.ts \
    pinLength1_en.ts \

OTHER_FILES += pinLength1_zh.ts \
    pinLength1_en.ts \
    pinLength1.ini\
pinLength1.ver\

HEADERS += pinLength1Wdg.h \
    pinLength1Mod.h \
    pinLength1Fact.h \
    pinLength1Param.h \

SOURCES += pinLength1Wdg.cpp \
    pinLength1Mod.cpp \
    pinLength1Fact.cpp \
    pinLength1Param.cpp \

FORMS += pinLength1Wdg.ui

RESOURCES += pinLength1.qrc

