QT += qml
QT -= gui

CONFIG   += console
CONFIG   -= app_bundle

# Add more folders to ship with the application, here
qml_folder.source = qml
qml_folder.target = .
js_folder.source = js
js_folder.target = .
DEPLOYMENTFOLDERS = qml_folder js_folder

MAINPROFILEPWD = $$PWD

# Necessary to copy files to build folder.
# Adapted for console from qtquick2applicationviewer.
copyCommand =
for(deploymentfolder, DEPLOYMENTFOLDERS) {
    source = $$MAINPROFILEPWD/$$eval($${deploymentfolder}.source)
    source = $$replace(source, \\\\, /)
    target = $$OUT_PWD/$$eval($${deploymentfolder}.target)
    target = $$replace(target, \\\\, /)
    sourcePathSegments = $$split(source, /)
    targetFullPath = $$target/$$last(sourcePathSegments)
    targetFullPath ~= s,/\\.?/,/,
    !isEqual(source,$$targetFullPath) {
        !isEmpty(copyCommand):copyCommand += &&
        copyCommand += $(MKDIR) \"$$target\"
        copyCommand += && $(COPY_DIR) \"$$source\" \"$$target\"
    }
}
!isEmpty(copyCommand) {
    copyCommand = @echo Copying application data... && $$copyCommand
    copydeploymentfolders.commands = $$copyCommand
    first.depends = $(first) copydeploymentfolders
    export(first.depends)
    export(copydeploymentfolders.commands)
    QMAKE_EXTRA_TARGETS += first copydeploymentfolders
}
!isEmpty(target.path) {
    installPrefix = $${target.path}
} else {
    installPrefix = /opt/$${TARGET}
}
for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemfiles = $${item}.files
    $$itemfiles = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath = $${installPrefix}/$$eval($${deploymentfolder}.target)
    export($$itemfiles)
    export($$itempath)
    INSTALLS += $$item
}

isEmpty(target.path) {
    target.path = $${installPrefix}/bin
    export(target.path)
}
INSTALLS += target

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp

include(../commonjs/commonjs.pri)

OTHER_FILES += \
    js/main.js

