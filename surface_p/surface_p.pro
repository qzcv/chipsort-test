TARGET = surface_p
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += surface_p_zh.ts \
    surface_p_en.ts \

OTHER_FILES += surface_p_zh.ts \
    surface_p_en.ts \
    surface_p.ini\
surface_p.ver\

HEADERS += surface_pWdg.h \
    surface_pMod.h \
    surface_pFact.h \
    surface_pParam.h \

SOURCES += surface_pWdg.cpp \
    surface_pMod.cpp \
    surface_pFact.cpp \
    surface_pParam.cpp \

FORMS += surface_pWdg.ui

RESOURCES += surface_p.qrc

