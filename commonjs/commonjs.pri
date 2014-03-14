SOURCES += \
    $$PWD/commonjs.cpp \
    ../commonjs/cjsprocess.cpp \
    ../commonjs/cjsfile.cpp

HEADERS += \
    $$PWD/commonjs.h \
    ../commonjs/cjsprocess.h \
    ../commonjs/cjsfile.h

OTHER_FILES += \
    $$PWD/templates/require.qml

RESOURCES += \
    $$PWD/resources.qrc

INCLUDEPATH += $$PWD
