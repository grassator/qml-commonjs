#include "commonjs_plugin.h"
#include "commonjs.h"

#include <qqml.h>

void CommonJSPlugin::registerTypes(const char *uri)
{
    Q_INIT_RESOURCE(resources);
    // @uri CommonJS
    qmlRegisterSingletonType<CommonJS>(uri, 1, 0, "CommonJS", CommonJS::singletonProvider);
}

CommonJSPlugin::~CommonJSPlugin()
{
    Q_CLEANUP_RESOURCE(resources);
}


