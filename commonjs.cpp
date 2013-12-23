#include "commonjs.h"

/**
 * @brief Provides node.js require functionality from C++ or QML
 * @param url
 * @return
 */
QJSValue CommonJS::require(QString url)
{
    QString p;
    p =     "function doRequire(resolvedUrl) { \n"
            // first checking cache
            "  var cached = CommonJS.__cachedRequire(resolvedUrl); \n"
            "  if(typeof(cached) !== 'undefined') { \n"
            "    return cached; \n"
            "  } \n"

            // Setting up expected global variables
            "  var module = { exports: {} }; \n"
            "  var exports = module.exports; \n"

            // for nested require calls we need to adjust
            // base path so that relative require calls work
            "  var require = (function(base, url) { \n"
            "    url = CommonJS.resolvedUrl(url, base); \n"
            "    return doRequire(url); \n"
            "  }).bind(this, resolvedUrl); \n"

            // Evaluating require'd code. It would be better
            // to use QJSEngine::evaluate but unfortunately
            // it doesn't support nested calls
            "  var code = CommonJS.__loadFile(resolvedUrl); \n"
            "  eval(code); \n"

            // Adding to cache
            "  CommonJS.__addCachedRequire(resolvedUrl, module.exports); "

            // Providing user with the result
            "  return module.exports; \n"
            "} \n"

            // bootstrapping
            "doRequire('%1'); \n";

    p = p.arg(resolvedUrl(url));
    return m_engine->evaluate(p, "snippet");
}

QJSValue CommonJS::__cachedRequire(QString resolvedUrl)
{
    return m_cache.value(resolvedUrl, QJSValue(QJSValue::UndefinedValue));
}

void CommonJS::__addCachedRequire(QString resolvedUrl, QJSValue value)
{
    m_cache.insert(resolvedUrl, value);
}

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
QString CommonJS::resolvedUrl(QString url, QString base)
{
    if(base.isEmpty()) {
        // This is the easiest way to get resolved url
        // since there's no direct access to Qt.resolvedUrl method
        QString program = QString("Qt.resolvedUrl('%1')").arg(url);
        url = m_engine->evaluate(program).toVariant().toUrl().toLocalFile();
    } else {
        if(url.left(2) == "./") {
            url = QFileInfo(base).absolutePath() + url.mid(1);
        }
    }
    return url;
}
