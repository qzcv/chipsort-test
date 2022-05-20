TARGET = edgeJudge
TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../modules
include(../../station.pri)
# Input
TRANSLATIONS += edgeJudge_zh.ts \
    edgeJudge_en.ts \

OTHER_FILES += edgeJudge_zh.ts \
    edgeJudge_en.ts \
    edgeJudge.ini\
edgeJudge.ver\

HEADERS += edgeJudgeWdg.h \
    edgeJudgeMod.h \
    edgeJudgeFact.h \
    edgeJudgeParam.h \

SOURCES += edgeJudgeWdg.cpp \
    edgeJudgeMod.cpp \
    edgeJudgeFact.cpp \
    edgeJudgeParam.cpp \

FORMS += edgeJudgeWdg.ui

RESOURCES += edgeJudge.qrc

