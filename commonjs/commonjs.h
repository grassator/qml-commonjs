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
    Q_PROPERTY(QJSValue process READ process)

public:
    explicit CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE QString resolve(QString url, QString base = "");
    Q_INVOKABLE QJSValue require(QString url);
    QJSValue cache() const { return m_cache; }
    QJSValue global() const { return m_global; }
    QJSValue process() const { return m_process; }

    // Pseudo-private function that is only public
    // so that javascript code will have access to them
    Q_INVOKABLE QString __loadFile(QString url);

    Q_INVOKABLE int setTimeout(QJSValue callback = QJSValue(), int delay = 0);
    Q_INVOKABLE void clearTimeout(int timeoutId = 0);
    Q_INVOKABLE int setInterval(QJSValue callback = QJSValue(), int interval = 0);
    Q_INVOKABLE void clearInterval(int intervalId = 0);

    // QML singleton provider for usage with qmlRegisterSingletonType<>
    static QObject* singletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        return new CommonJS(engine, scriptEngine);
    }

    // Convenince function for registering CommonJS in QML
    static void registerQmlSingletonType(const char *uri = "CommonJS")
    {
        // @uri CommonJS
        qmlRegisterSingletonType<CommonJS>(uri, majorVersion, minorVersion, "CommonJS", singletonProvider);
    }

    static int majorVersion;
    static int minorVersion;

protected:
    QString tryModuleUrlAsDirectory(QString &url);
    void initRequireJSCode();
    QHash<int, QJSValue> m_setTimeoutCallbacks;
    QHash<int, QJSValue> m_setIntervalCallbacks;
    QJSValue m_process;
    QStringList m_builtInModules;
    QJSValue m_global;
    QJSValue m_cache;
    QJSValue m_require;
    QQmlEngine *m_engine;
    QJSEngine *m_scriptEngine;

    // overloaded methods
    void timerEvent(QTimerEvent *event);

};

#endif // COMMONJS_H

