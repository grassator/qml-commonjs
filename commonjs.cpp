#include "commonjs.h"
#include <QDebug>

CommonJS::CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine)
    : QObject(NULL), m_engine(engine), m_scriptEngine(scriptEngine)
{
    m_requireTemplate = __loadFile(":/templates/require.js");
}


/**
 * @brief Provides node.js require functionality from C++ or QML
 * @param url
 * @return
 */
QJSValue CommonJS::require(QString url)
{
    QString p(m_requireTemplate);
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
