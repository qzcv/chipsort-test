TARGET = composeND_pre
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += composeND_pre_zh.ts \
    composeND_pre_en.ts \

OTHER_FILES += composeND_pre_zh.ts \
    composeND_pre_en.ts \
    composeND_pre.ini\
composeND_pre.ver\

HEADERS += composeND_preWidget.h \
    composeND_preModule.h \
    composeND_preFactory.h \
    composeND_preParam.h \

SOURCES += composeND_preWidget.cpp \
    composeND_preModule.cpp \
    composeND_preFactory.cpp \
    composeND_preParam.cpp \

FORMS += composeND_preWidget.ui

RESOURCES += composeND_pre.qrc

