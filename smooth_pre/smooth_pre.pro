TARGET = smooth_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += smooth_pre_zh.ts \
    smooth_pre_en.ts \

OTHER_FILES += smooth_pre_zh.ts \
    smooth_pre_en.ts \
    smooth_pre.ini\
smooth_pre.ver\

HEADERS += smooth_preWdg.h \
    smooth_preMod.h \
    smooth_preFact.h \
    smooth_preParam.h \

SOURCES += smooth_preWdg.cpp \
    smooth_preMod.cpp \
    smooth_preFact.cpp \
    smooth_preParam.cpp \

FORMS += smooth_preWdg.ui

RESOURCES += smooth_pre.qrc

