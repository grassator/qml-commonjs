#ifndef COMMONJS_H
#define COMMONJS_H

#include <QObject>
#include <QtQml>

class CommonJS : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CommonJS)
    Q_PROPERTY(QJSValue cache READ cache)
    Q_PROPERTY(QJSValue global READ global)

public:
    explicit CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE QString resolve(QString url, QString base = "");
    Q_INVOKABLE QJSValue require(QString url);
    QJSValue cache() const { return m_cache; }
    QJSValue global() const { return m_global; }

    // Pseudo-private function that is only public
    // so that javascript code will have access to them
    Q_INVOKABLE QString __loadFile(QString url);

    // QML singleton provider for usage with qmlRegisterSingletonType<>
    static QObject* singletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        return new CommonJS(engine, scriptEngine);
    }

    // Convenince function for registering CommonJS in QML
    static void registerQmlSingletonType(const char *uri = "CommonJS")
    {
        qmlRegisterSingletonType<CommonJS>(uri, 0, 1, "CommonJS", singletonProvider);
    }


protected:
    void initRequireJSCode();

    QJSValue m_global;
    QJSValue m_cache;
    QJSValue m_require;
    QQmlEngine *m_engine;
    QJSEngine *m_scriptEngine;
};

#endif // COMMONJS_H

