#ifndef CJSPROCESS_H
#define CJSPROCESS_H

#include <QObject>
#include <QCoreApplication>
#include <QStringList>
#include <QVariantMap>

class CJSProcess : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList argv READ argv)
    Q_PROPERTY(QString execPath READ execPath)
    Q_PROPERTY(QStringList execArgv READ execArgv)
    Q_PROPERTY(QVariantMap env READ env)

public:
    explicit CJSProcess(QObject *parent = 0) : QObject(parent) {}

    QStringList argv() const { return qApp->arguments(); }
    QString execPath() const { return qApp->applicationFilePath(); }

    // Since we don't support node-specific arguments this list is always empty
    QStringList execArgv() const { return QStringList(); }

    /* This may not do what common js modules expect according to nodejs docs:
     * "This causes node to emit an abort.
     *  This will cause node to exit and generate a core file."
     */
    Q_INVOKABLE void abort() const { exit(-1); }

    QVariantMap env();

};

#endif // CJSPROCESS_H
