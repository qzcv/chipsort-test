TARGET = segLead
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += segLead_zh.ts \
    segLead_en.ts \

OTHER_FILES += segLead_zh.ts \
    segLead_en.ts \
    segLead.ini\
segLead.ver\

HEADERS += segLeadWdg.h \
    segLeadMod.h \
    segLeadFact.h \
    segLeadParam.h \

SOURCES += segLeadWdg.cpp \
    segLeadMod.cpp \
    segLeadFact.cpp \
    segLeadParam.cpp \

FORMS += segLeadWdg.ui

RESOURCES += segLead.qrc

