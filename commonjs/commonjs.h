#ifndef COMMONJS_H
#define COMMONJS_H

#include <QObject>
#include <QtQml>

class CJSFile;

class CommonJS : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CommonJS)
    Q_PROPERTY(QJSValue cache READ cache)
    Q_PROPERTY(QJSValue global READ global)
    Q_PROPERTY(QJSValue process READ process)
    Q_PROPERTY(QJSValue resolve READ resolve)

public:
    explicit CommonJS(QQmlEngine *engine, QJSEngine *scriptEngine);


    Q_INVOKABLE QJSValue require(QString url);
    QJSValue resolve() const { return m_resolve; }
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
    QJSValue binding(QString name) const;

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

    QJSEngine *scriptEngine() const { return m_scriptEngine; }

protected:
    void initRequireJSCode();
    QHash<int, QJSValue> m_setTimeoutCallbacks;
    QHash<int, QJSValue> m_setIntervalCallbacks;
    QJSValue m_process, m_global, m_cache, m_require, m_resolve, m_fsStats;
    QQmlEngine *m_engine;
    QJSEngine *m_scriptEngine;
    CJSFile *m_cjsFile;

    // overloaded methods
    void timerEvent(QTimerEvent *event);

};

#endif // COMMONJS_H

