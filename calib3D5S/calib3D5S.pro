TARGET = calib3D5S
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += calib3D5S_zh.ts \
    calib3D5S_en.ts \

OTHER_FILES += calib3D5S_zh.ts \
    calib3D5S_en.ts \
    calib3D5S.ini\
calib3D5S.ver\

HEADERS += calib3D5SWdg.h \
    calib3D5SMod.h \
    calib3D5SFact.h \
    calib3D5SParam.h \

SOURCES += calib3D5SWdg.cpp \
    calib3D5SMod.cpp \
    calib3D5SFact.cpp \
    calib3D5SParam.cpp \

FORMS += calib3D5SWdg.ui

RESOURCES += calib3D5S.qrc

