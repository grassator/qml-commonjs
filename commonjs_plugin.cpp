#include "commonjs_plugin.h"
#include "commonjs.h"

#include <qqml.h>

static QObject* CommonJSProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    return new CommonJS(engine, scriptEngine);
}

void CommonJSPlugin::registerTypes(const char *uri)
{
    Q_INIT_RESOURCE(resources);
    // @uri CommonJS
    qmlRegisterSingletonType<CommonJS>(uri, 1, 0, "CommonJS", CommonJSProvider);
}

CommonJSPlugin::~CommonJSPlugin()
{
    Q_CLEANUP_RESOURCE(resources);
}


