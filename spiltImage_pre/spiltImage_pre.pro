TARGET = spiltImage_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += spiltImage_pre_zh.ts \
    spiltImage_pre_en.ts \

OTHER_FILES += spiltImage_pre_zh.ts \
    spiltImage_pre_en.ts \
    spiltImage_pre.ini\
spiltImage_pre.ver\

HEADERS += spiltImage_preWdg.h \
    spiltImage_preMod.h \
    spiltImage_preFact.h \
    spiltImage_preParam.h \

SOURCES += spiltImage_preWdg.cpp \
    spiltImage_preMod.cpp \
    spiltImage_preFact.cpp \
    spiltImage_preParam.cpp \

FORMS += spiltImage_preWdg.ui

RESOURCES += spiltImage_pre.qrc

