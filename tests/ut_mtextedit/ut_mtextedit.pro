include(../common_top.pri)

TARGET = ut_mtextedit

contains(DEFINES, HAVE_MALIIT) {
    CONFIG += link_pkgconfig
    PKGCONFIG += maliit-1.0
}

SOURCES += \
    ut_mtextedit.cpp \
    $$STUBSDIR/stubbase.cpp \

HEADERS += \
    $$STUBSDIR/mpannableviewport_stub.h \
    $$STUBSDIR/mpannablewidget_stub.h \
    $$STUBSDIR/mtexteditview_stub.h \
    $$STUBSDIR/mtheme_stub.h \
    $$STUBSDIR/mwidgetview_stub.h \
    ut_mtextedit.h \

INCLUDEPATH += \
    $$MSRCDIR/corelib/widgets/ \
    $$MSRCDIR/../tests/ \
             
include(../common_bot.pri)
