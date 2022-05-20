TARGET = angleFilter
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += angleFilter_zh.ts \
    angleFilter_en.ts \

OTHER_FILES += angleFilter_zh.ts \
    angleFilter_en.ts \
    angleFilter.ini\
angleFilter.ver\

HEADERS += angleFilterWdg.h \
    angleFilterMod.h \
    angleFilterFact.h \
    angleFilterParam.h \

SOURCES += angleFilterWdg.cpp \
    angleFilterMod.cpp \
    angleFilterFact.cpp \
    angleFilterParam.cpp \

FORMS += angleFilterWdg.ui

RESOURCES += angleFilter.qrc

