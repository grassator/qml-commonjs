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
    initRequireJSCode();
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
 * @brief Calls a JS function after a specified delay.
 * @param callback JS function
 * @param delay
 * @return
 */
int CommonJS::setTimeout(QJSValue callback, int delay)
{
    if(delay < 0) {
        delay = 0;
    }

    int timerId = startTimer(delay, Qt::PreciseTimer);
    m_setTimeoutCallbacks.insert(timerId, callback);

    return timerId;
}

/**
 * @brief Clears delayed function or code snippet set by setTimeout
 * @param timeoutId
 * @return
 */
void CommonJS::clearTimeout(int timeoutId)
{
    if(m_setTimeoutCallbacks.contains(timeoutId)) {
        m_setTimeoutCallbacks.remove(timeoutId);
        killTimer(timeoutId);
    }
}

/**
 * @brief Calls a JS function with a specified interval.
 * @param callback JS function
 * @param delay
 * @return
 */
int CommonJS::setInterval(QJSValue callback, int interval)
{
    if(interval < 0) {
        interval = 0;
    }

    int timerId = startTimer((interval < 0), Qt::PreciseTimer);
    m_setIntervalCallbacks.insert(timerId, callback);

    return timerId;
}


/**
 * @brief Clears interval function or code snippet set by setInterval
 * @param timeoutId
 * @return
 */
void CommonJS::clearInterval(int intervalId)
{
    if(m_setIntervalCallbacks.contains(intervalId)) {
        m_setIntervalCallbacks.remove(intervalId);
        killTimer(intervalId);
    }
}

/**
 * @brief Used for implementation of setTimeout / setInterval
 * @param event
 * @internal
 */
void CommonJS::timerEvent(QTimerEvent *event) {
    QJSValue callback;
    if(m_setTimeoutCallbacks.contains(event->timerId())) {
        callback = m_setTimeoutCallbacks[event->timerId()];
        m_setTimeoutCallbacks.remove(event->timerId());
        killTimer(event->timerId());
        event->accept();
    } else if(m_setIntervalCallbacks.contains(event->timerId())) {
        callback = m_setIntervalCallbacks[event->timerId()];
        event->accept();
    }

    if(!callback.isUndefined()) {
        if(callback.isCallable()) {
            callback.call();
        } else { // if isn't a function callback is treated as snippet of code
            m_scriptEngine->evaluate(callback.toString());
        }
    }
}

QString CommonJS::tryModuleUrlAsDirectory(QString &url)
{
    // module as folder
    QFileInfo info(url);
    if(!info.isDir()) {
        return url;
    }

    QString packagePath = url + "/package.json";
    if(QFile::exists(packagePath)) {
        QFile jsonFile(packagePath);
        if(jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&jsonFile);
            QJsonDocument package = QJsonDocument::fromJson(stream.readAll().toUtf8());
            if(package.isObject() && package.object().contains("main")) {
                url = url + "/" + package.object().value("main").toString();
            }
            jsonFile.close();
        }
    } else {
        url = url + "/index.js";
    }

    return url;
}

/**
 * @brief Provides resolved url based on node.js rules
 * but optionally takes base parameter.
 * @param url
 * @param base
 * @return
 */
//QJSValue CommonJS::resolve(QString url, QString base)
//{
//    QString originalUrl = url;

//    // Making sure that our base path is a directory
//    if(!base.isEmpty()) {
//        QFileInfo baseDirInfo(base);
//        if(baseDirInfo.suffix() == "js") {
//            base = baseDirInfo.dir().absolutePath();
//        }
//    }

//    // Shortcurcuiting for built-in modules
//    if(m_builtInModules.contains(url)) {
//        return ":/lib/" + url + ".js";
//    }

//    // removing prefix from file urls if present
//    if(url.left(7) == "file://") {
//        url = url.mid(7);
//    }

//    // resolving relative path
//    if(url.left(2) == "./" || url.left(3) == "../") {
//        // trying as file
//        url = QDir::cleanPath(base + "/" + url);
//        url = tryModuleUrlAsDirectory(url);
//    } else {
//        // Else if not absolute or qrc path then try recursive
//        // resolving of node_modules folders
//        if(url.at(0) != '/' && url.left(2) != ":/") {
//            if(base.isEmpty()) {
//                base = QFileInfo(url).dir().absolutePath();
//            }
//            QString tempUrl = base + "/node_modules/" + url;
//            url = tryModuleUrlAsDirectory(tempUrl);
//        } else {
//            url = tryModuleUrlAsDirectory(url);
//        }
//    }

//    QFileInfo info(url);
//    if(info.isReadable() && info.isFile() && info.suffix() == "js") {
//        return url;
//    } else {
//        QString error("new Error('Cannot find module \\'%1\\'')");
//        return m_scriptEngine->evaluate(error.arg(originalUrl));
//    }
//}

/**
 * @brief Returns QJSValue representing required js file module.exports
 * @param url
 * @return
 */
QJSValue CommonJS::require(QString url)
{
    // Creating a fake parent module to avoid additional logic inside
    // main require function to deal with requiring from QML code
    QString base = m_scriptEngine->evaluate("Qt.resolvedUrl('.')").toString();
    QJSValue qmlParentModule = m_scriptEngine->newObject();
    qmlParentModule.setProperty("id", base);
    qmlParentModule.setProperty("children", m_scriptEngine->newArray());

    QJSValue result = m_require.call(QJSValueList() << url << qmlParentModule);
    if(result.isError()) {
        m_scriptEngine->evaluate("console.error").call(QJSValueList() << result);
    }
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
    QJSValue jsObj = m_engine->newQObject(obj);

    // Getting compiled version of require function from QML object
    m_require = jsObj.property("requireInitializer");
    m_resolve = jsObj.property("resolveInitializer");

    // Making CommonJS singleton available inside require and resolve function
    // to be able to refer back to it without relying on CommonJS QML modul
    // being imported into global namespace (without `as SomeIdentifier`)
    m_require = m_require.call(QJSValueList() << m_scriptEngine->newQObject(this));
    m_resolve = m_resolve.call(QJSValueList() << m_scriptEngine->newQObject(this));
}
