#ifndef CJSPROCESS_H
#define CJSPROCESS_H

#include <QObject>
#include <QCoreApplication>
#include <QStringList>
#include <QVariantMap>
#include <QJSValue>
#include <QDir>
#include "commonjs.h"

class CJSProcess : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList argv READ argv)
    Q_PROPERTY(QString execPath READ execPath)
    Q_PROPERTY(QStringList execArgv READ execArgv)
    Q_PROPERTY(QVariantMap env READ env)
    Q_PROPERTY(QString version READ version)
    Q_PROPERTY(QString platform READ platform)

public:
    explicit CJSProcess(CommonJS *main = 0)
        : QObject(main), m_main(main), m_tickEventPosted(false) {}

    QStringList argv() const { return qApp->arguments(); }
    QString execPath() const { return qApp->applicationFilePath(); }
    Q_INVOKABLE QString pwd() const { return QDir::currentPath(); }
    Q_INVOKABLE void chdir(QString path) const { QDir::setCurrent(path); }

    // Since we don't support node-specific arguments this list is always empty
    QStringList execArgv() const { return QStringList(); }

    Q_INVOKABLE QJSValue binding(QString name) { return m_main->binding(name); }
    QString version() const;
    QString platform() const;
    QVariantMap env();

    /* This may not do what common js modules expect according to nodejs docs:
     * "This causes node to emit an abort.
     *  This will cause node to exit and generate a core file."
     */
    Q_INVOKABLE void abort() const { exit(1); }

    Q_INVOKABLE void exit(int code = 0) const { qApp->exit(code); }

#ifndef Q_WS_WIN
    Q_INVOKABLE int getgid() const;
    Q_INVOKABLE void setgid(int gid) const;
    Q_INVOKABLE int getuid() const;
    Q_INVOKABLE void setuid(int uid) const;
#endif

    Q_INVOKABLE void nextTick(QJSValue callback);

protected:
    CommonJS *m_main;
    bool m_tickEventPosted;
    QList<QJSValue> m_nextTickCallbacks;
    void customEvent(QEvent *e);

};

#endif // CJSPROCESS_H
