DEPENDS=
contains( DUI_BUILD_FEATURES, coverage ) {
    include(coverage.pri)
}

QMAKE_CLEAN += \
    *.gcda \
    *.gcno \
