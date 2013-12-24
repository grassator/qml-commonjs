#include "commonjs.h"
#include <QDebug>

CommonJS::CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine)
    : QObject(NULL), m_engine(engine), m_scriptEngine(scriptEngine)
{
    m_cache = engine->newObject();
    m_global = engine->newObject();
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
QString CommonJS::resolve(QString url, QString base)
{
    if(base.isEmpty()) {
        // This is the easiest way to get resolved url
        // since there's no direct access to Qt.resolvedUrl method
        QString program = QString("Qt.resolvedUrl('%1')").arg(url);
        url = m_engine->evaluate(program).toVariant().toUrl().toLocalFile();
    }

    if(url.left(2) == "./" || url.left(3) == "../") { // relative path
        url = QFileInfo(base).absolutePath() + "/" + url.mid(1);
        url = QDir::cleanPath(url);
    } else if(url.at(0) != '/') { // not relative or absolute

    }
    return url;
}

/**
 * @brief Returns QJSValue containing compiled version
 * of 'require' CommonJS function.
 * @return
 */
QJSValue CommonJS::require()
{
    if(m_require.isUndefined()) {
        QString requireCode = __loadFile(":/templates/require.js");
        m_require = m_engine->evaluate("(" + requireCode + ")");

        // Making CommonJS singleton availabe inside require
        // function to be able to refer back to it without
        // relying on CommonJS QML module being imported into
        // global namespace (without `as SomeIdentifier`)
        m_require.setProperty("__native", m_engine->newQObject(this));
    }
    return m_require;
}
