SOURCES += \
    $$PWD/commonjs.cpp \
    ../commonjs/cjsprocess.cpp

HEADERS += \
    $$PWD/commonjs.h \
    ../commonjs/cjsprocess.h

OTHER_FILES += \
    $$PWD/templates/require.qml \
    $$PWD/lib/assert.js \
    $$PWD/lib/util.js

RESOURCES += \
    $$PWD/resources.qrc

INCLUDEPATH += $$PWD
