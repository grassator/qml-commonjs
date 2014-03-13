#ifndef CJSFILE_H
#define CJSFILE_H

#include <QObject>
#include <QJSValue>
#include "commonjs.h"

class CJSFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QJSValue Stats READ StatsConstuctor WRITE setStatsConstuctor)
    Q_PROPERTY(QJSValue stat READ stat)


public:
    explicit CJSFile(CommonJS *main);

    QJSValue StatsConstuctor() const { return m_StatsConstructor; }

    QJSValue stat() const { return m_stat; }
    Q_INVOKABLE int doStat(QString path, QJSValue callback);

signals:

public slots:
    void setStatsConstuctor(QJSValue jsFunc) { m_StatsConstructor = jsFunc; }

protected:
    CommonJS *m_main;
    QJSValue m_stat, m_StatsConstructor;
};

#endif // CJSFILE_H
