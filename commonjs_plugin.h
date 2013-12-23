#ifndef COMMONJS_PLUGIN_H
#define COMMONJS_PLUGIN_H

#include <QQmlExtensionPlugin>

class CommonJSPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // COMMONJS_PLUGIN_H

