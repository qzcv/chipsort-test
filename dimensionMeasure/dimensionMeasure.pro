TARGET = dimensionMeasure
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += dimensionMeasure_zh.ts \
    dimensionMeasure_en.ts \

OTHER_FILES += dimensionMeasure_zh.ts \
    dimensionMeasure_en.ts \
    dimensionMeasure.ini\
dimensionMeasure.ver\

HEADERS += dimensionMeasureWdg.h \
    dimensionMeasureMod.h \
    dimensionMeasureFact.h \
    dimensionMeasureParam.h \

SOURCES += dimensionMeasureWdg.cpp \
    dimensionMeasureMod.cpp \
    dimensionMeasureFact.cpp \
    dimensionMeasureParam.cpp \

FORMS += dimensionMeasureWdg.ui

RESOURCES += dimensionMeasure.qrc

