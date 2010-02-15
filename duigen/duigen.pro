include(../mkspecs/common.pri)

TEMPLATE = app
TARGET = duigen
target.path = $$DUI_INSTALL_BIN
INCLUDEPATH += .
DEPENDPATH += $$INCLUDEPATH

# enable QString optimizations
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS

# Check for mixing of const and non-const iterators, 
# which can cause problems when built with some compilers:
DEFINES += QT_STRICT_ITERATORS

win32-msvc*:{
    target.commands += copy release\duigen.exe .
}

# Dont generate duigen.app bundle for Mac OS X
macx:CONFIG -= app_bundle
CONFIG += \
    qt \

SOURCES += \
    duigen.cpp \
    duigen_processor.cpp \

HEADERS += \


INSTALLS += \
     target\

QMAKE_EXTRA_TARGETS += check
check.depends = $${TARGET}
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends = $${TARGET}
check-xml.commands = $$system(true)
