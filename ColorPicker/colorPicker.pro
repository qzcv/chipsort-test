TARGET = ColorPicker
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += colorPicker_zh.ts \
    colorPicker_en.ts \

OTHER_FILES += colorPicker_zh.ts \
    colorPicker_en.ts \
    ColorPicker.ini\
ColorPicker.ver\

HEADERS += colorPickerWdg.h \
    colorPickerMod.h \
    colorPickerFact.h \
    colorPickerParam.h \

SOURCES += colorPickerWdg.cpp \
    colorPickerMod.cpp \
    colorPickerFact.cpp \
    colorPickerParam.cpp \

FORMS += colorPickerWdg.ui

RESOURCES += colorPicker.qrc

