include(../mkspecs/common.pri)

TEMPLATE = subdirs
SUBDIRS =

QMAKE_EXTRA_TARGETS += check
check.depends =
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += check-xml
check-xml.depends =
check-xml.commands = $$system(true)

duimoc.target = .dummy
duimoc.commands = touch $$duimoc.target
duimoc.path = $$DUI_INSTALL_BIN
duimoc.files = duimoc

INSTALLS += \
     duimoc\
