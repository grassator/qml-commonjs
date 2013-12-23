#ifndef COMMONJS_H
#define COMMONJS_H

#include <QObject>
#include <QtQml>

class CommonJS : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CommonJS)

public:
    explicit CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine)
        : QObject(NULL), m_engine(engine), m_scriptEngine(scriptEngine) {}

    Q_INVOKABLE QJSValue require(QString url);
    Q_INVOKABLE QString resolvedUrl(QString url, QString base = "");

    // Pseudo-private function that are only public
    // so that javascript code will have access to them
    Q_INVOKABLE QJSValue __cachedRequire(QString resolvedUrl);
    Q_INVOKABLE void __addCachedRequire(QString resolvedUrl, QJSValue value);
    Q_INVOKABLE QString __loadFile(QString url);

protected:
    QQmlEngine *m_engine;
    QJSEngine *m_scriptEngine;
    QHash <QString, QJSValue> m_cache;
};

#endif // COMMONJS_H

