#include "commonjs_plugin.h"
#include "commonjs.h"

#include <qqml.h>

void CommonJSPlugin::registerTypes(const char *uri)
{
    Q_INIT_RESOURCE(resources);
    CommonJS::registerQmlSingletonType(uri);
}

CommonJSPlugin::~CommonJSPlugin()
{
    Q_CLEANUP_RESOURCE(resources);
}


