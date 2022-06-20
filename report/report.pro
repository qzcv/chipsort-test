TARGET = GRRreport
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += GRRreport_zh.ts \
    GRRreport_en.ts \

OTHER_FILES += GRRreport_zh.ts \
    GRRreport_en.ts \
    GRRreport.ini\
GRRreport.ver\

HEADERS += GRRreportWdg.h \
    GRRreportMod.h \
    GRRreportFact.h \
    GRRreportParam.h \

SOURCES += GRRreportWdg.cpp \
    GRRreportMod.cpp \
    GRRreportFact.cpp \
    GRRreportParam.cpp \

FORMS += GRRreportWdg.ui

RESOURCES += GRRreport.qrc

