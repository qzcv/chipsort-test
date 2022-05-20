TARGET = arithmetic_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += arithmetic_pre_zh.ts \
    arithmetic_pre_en.ts \

OTHER_FILES += arithmetic_pre_zh.ts \
    arithmetic_pre_en.ts \
    arithmetic_pre.ini\
arithmetic_pre.ver\

HEADERS += arithmetic_preWidget.h \
    arithmetic_preModule.h \
    arithmetic_preFactory.h \
    arithmetic_preParam.h \

SOURCES += arithmetic_preWidget.cpp \
    arithmetic_preModule.cpp \
    arithmetic_preFactory.cpp \
    arithmetic_preParam.cpp \

FORMS += arithmetic_preWidget.ui

RESOURCES += arithmetic_pre.qrc

