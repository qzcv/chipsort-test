TARGET = locationGui
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += locationGui_zh.ts \
    locationGui_en.ts \

OTHER_FILES += locationGui_zh.ts \
    locationGui_en.ts \
    locationGui.ini\
locationGui.ver\

HEADERS += locationGuiWdg.h \
    locationGuiMod.h \
    locationGuiFact.h \
    locationGuiParam.h \

SOURCES += locationGuiWdg.cpp \
    locationGuiMod.cpp \
    locationGuiFact.cpp \
    locationGuiParam.cpp \

FORMS += locationGuiWdg.ui

RESOURCES += locationGui.qrc

