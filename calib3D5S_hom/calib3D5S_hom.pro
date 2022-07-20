TARGET = calib3D5S_hom
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += calib3D5S_hom_zh.ts \
    calib3D5S_hom_en.ts \

OTHER_FILES += calib3D5S_hom_zh.ts \
    calib3D5S_hom_en.ts \
    calib3D5S_hom.ini\
calib3D5S_hom.ver\

HEADERS += calib3D5S_homWdg.h \
    calib3D5S_homMod.h \
    calib3D5S_homFact.h \
    calib3D5S_homParam.h \

SOURCES += calib3D5S_homWdg.cpp \
    calib3D5S_homMod.cpp \
    calib3D5S_homFact.cpp \
    calib3D5S_homParam.cpp \

FORMS += calib3D5S_homWdg.ui

RESOURCES += calib3D5S_hom.qrc

