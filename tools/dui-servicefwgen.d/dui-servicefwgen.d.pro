include(../../mkspecs/common.pri)

TEMPLATE = app
TARGET = ../dui-servicefwgen
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += dui-servicefwgen.cpp

target.path = $$DUI_INSTALL_BIN
INSTALLS += target
