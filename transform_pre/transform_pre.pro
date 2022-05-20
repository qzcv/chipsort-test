TARGET = transform_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += transform_pre_zh.ts \
    transform_pre_en.ts \

OTHER_FILES += transform_pre_zh.ts \
    transform_pre_en.ts \
    transform_pre.ini\
transform_pre.ver\

HEADERS += transform_preWdg.h \
    transform_preMod.h \
    transform_preFact.h \
    transform_preParam.h \

SOURCES += transform_preWdg.cpp \
    transform_preMod.cpp \
    transform_preFact.cpp \
    transform_preParam.cpp \

FORMS += transform_preWdg.ui

RESOURCES += transform_pre.qrc

