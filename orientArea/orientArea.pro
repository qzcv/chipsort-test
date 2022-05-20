TARGET = orientArea
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += orientArea_zh.ts \
    orientArea_en.ts \

OTHER_FILES += orientArea_zh.ts \
    orientArea_en.ts \
    orientArea.ini\
orientArea.ver\

HEADERS += orientAreaWidget.h \
    orientAreaModule.h \
    orientAreaFactory.h \
    orientAreaParam.h \

SOURCES += orientAreaWidget.cpp \
    orientAreaModule.cpp \
    orientAreaFactory.cpp \
    orientAreaParam.cpp \

FORMS += orientAreaWidget.ui

RESOURCES += orientArea.qrc

