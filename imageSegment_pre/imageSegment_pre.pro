TARGET = imageSegment_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += imageSegment_pre_zh.ts \
    imageSegment_pre_en.ts \

OTHER_FILES += imageSegment_pre_zh.ts \
    imageSegment_pre_en.ts \
    imageSegment_pre.ini\
imageSegment_pre.ver\

HEADERS += imageSegment_preWdg.h \
    imageSegment_preMod.h \
    imageSegment_preFact.h \
    imageSegment_preParam.h \

SOURCES += imageSegment_preWdg.cpp \
    imageSegment_preMod.cpp \
    imageSegment_preFact.cpp \
    imageSegment_preParam.cpp \

FORMS += imageSegment_preWdg.ui

RESOURCES += imageSegment_pre.qrc

