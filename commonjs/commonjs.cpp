#include "commonjs.h"
#include "cjsprocess.h"
#include <QDebug>

/**
 * @brief Singleton class exposed to QML context
 * @param engine
 * @param scriptEngine
 */
CommonJS::CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine)
    : QObject(NULL), m_engine(engine), m_scriptEngine(scriptEngine)
{
    m_cache = m_scriptEngine->newObject();
    m_global = m_scriptEngine->newObject();
    m_process = m_scriptEngine->newQObject(new CJSProcess(this));

    m_builtInModules
            << "assert"
            << "util";
}

int CommonJS::minorVersion = 1;
int CommonJS::majorVersion = 0;

/**
 * @internal
 * @param url
 * @return
 */
QString CommonJS::__loadFile(QString url)
{
    QFile file(url);
    QString fileContent;

    if ( file.open(QIODevice::ReadOnly) ) {;
        QTextStream t( &file );
        fileContent = t.readAll();
        file.close();
    } else { // TODO check what node does in this case
        return QString();
    }

    return fileContent;
}

/**
 * @brief Provides resolved url based on node.js rules
 * but optionally takes base parameter.
 * @param url
 * @param base
 * @return
 */
QString CommonJS::resolve(QString url, QString base)
{
    // Shortcurcuiting for built-in modules
    if(m_builtInModules.contains(url)) {
        return ":/lib/" + url + ".js";
    }

    // removing prefix from file urls if present
    if(url.left(7) == "file://") {
        url = url.mid(7);
    }

    // resolving relative path
    if(url.left(2) == "./" || url.left(3) == "../") {
        // trying as file
        url = QDir::cleanPath(QFileInfo(base).absolutePath() + "/" + url);
        QFileInfo info(url);
        if(info.isDir()) {
            // FIXME Need to read in info here an use it
            if(QFile::exists(url + "/package.json")) {

            } else {
                url = url + "/index.js";
            }
        }
    } else {
        // Else if not absolute or qrc path then try more complex resolving
        if(url.at(0) != '/' && url.left(2) != ":/") {
            // FIXME need to implement this
        }
        return url;
    }
    return QString();
}

/**
 * @brief Returns QJSValue representing required js file module.exports
 * @param url
 * @return
 */
QJSValue CommonJS::require(QString url)
{
    if(!m_builtInModules.contains(url)) {
        // Getting resolved path relative to calling QML file
        QString program = QString("Qt.resolvedUrl('%1')").arg(url);
        url = m_scriptEngine->evaluate(program).toVariant().toUrl().toLocalFile();
    }

    if(m_require.isUndefined()) {
        initRequireJSCode();
    }

    QJSValue result = m_require.call(QJSValueList() << url);
    return result;
}

/**
 * @brief Compiles require js support code in an empty context
 * @internal
 */
void CommonJS::initRequireJSCode()
{
    // Creating component in an empty context so that required code
    // won't affect any global variables defined in calling context
    QQmlContext *context = new QQmlContext(m_engine);
    QQmlComponent component(m_engine);

    // Sandboxed version of require funciton is available
    // inside lightest possible QML component loaded here
    QString requireCode = __loadFile(":/templates/require.qml");

    // Now creating the component to compile our require functions
    component.setData(requireCode.toUtf8(), QUrl());
    QObject *obj = component.create(context);

    // Getting compiled version of require function from QML object
    m_require = m_engine->newQObject(obj).property("sandbox");

    // Making CommonJS singleton available inside require
    // function to be able to refer back to it without
    // relying on CommonJS QML module being imported into
    // global namespace (without `as SomeIdentifier`)
    m_require = m_require.call(QJSValueList() << m_scriptEngine->newQObject(this));
}
