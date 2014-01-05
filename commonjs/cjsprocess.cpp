#include "cjsprocess.h"
#include "commonjs.h"

// Gives access to process ENV
#include <stdio.h>
extern char **environ;

// Necessary for function related to gid / uid
#ifndef Q_WS_WIN
#include <sys/types.h>
#include <unistd.h>
#endif

/**
 * @brief This classed is used in implementation of nextTick() method
 */
class CJSTickEvent : public QEvent {
public:
    CJSTickEvent() : QEvent(CJSTickEvent::type()) {}

    static QEvent::Type type()
    {
        if (customEventType == QEvent::None)
        {
            int generatedType = QEvent::registerEventType();
            customEventType = static_cast<QEvent::Type>(generatedType);
        }
        return customEventType;
    }

private:
    static QEvent::Type customEventType;
};
QEvent::Type CJSTickEvent::customEventType = QEvent::None;



///////////////////////////////////////////////////////////////////////////////
// Start implementation of CJSProcess
///////////////////////////////////////////////////////////////////////////////

// POSIX-specific methods
#ifndef Q_WS_WIN
int CJSProcess::getgid() const { return getgid();}
void CJSProcess::setgid(int gid) const { setgid(gid); }
int CJSProcess::getuid() const { return getuid(); }
void CJSProcess::setuid(int uid) const { setuid(uid); }
#endif

void CJSProcess::nextTick(QJSValue callback = QJSValue())
{
    if(!callback.isCallable()) {
        return;
    }
    m_nextTickCallbacks.append(callback);

    // There's no point in posting multiple events
    if(!m_tickEventPosted) {
        // posting event with maximum priority because we want to be
        // processed before any other events (like IO)
        qApp->postEvent(this, new CJSTickEvent(), INT_MAX);
        m_tickEventPosted = true;
    }
}

/**
 * @brief Provides access to process ENV
 * @return
 */
QString CJSProcess::version() const
{
    return QString("%1.%2")
            .arg(CommonJS::majorVersion)
            .arg(CommonJS::minorVersion);
}

QString CJSProcess::platform() const
{
#ifdef Q_OS_WIN32
    return "win32";
#endif
#ifdef Q_OS_FREEBSD
    return "freebsd";
#endif
#ifdef Q_OS_DARWIN
    return "darwin";
#endif
#ifdef Q_OS_DARWIN64
    return "darwin";
#endif
#ifdef Q_OS_LINUX
    return "linux";
#endif
#ifdef Q_OS_SOLARIS
    return "sunos";
#endif
    return "unknown";
}

QVariantMap CJSProcess::env()
{
    QVariantMap envMap;
    char *s = *environ;
    for (int i = 0; s; i++) {
        QStringList pair = QString(s).split("=");
        envMap.insert(pair.first(), pair.last());
        s = *(environ+i);
    }
    return envMap;
}

/**
 * @brief Used to catch CJSTickEvent
 * @return
 */
void CJSProcess::customEvent(QEvent *e)
{
    if(e->type() == CJSTickEvent::type()) {
        // need to reset this so callbacks can call nextTick too
        m_tickEventPosted = false;

        // creating a copy of the list because otherwise
        // calls to nextTick() will be added to current list
        // of callbacks which may result in an infinite loop
        QList<QJSValue> callbacks = m_nextTickCallbacks;
        m_nextTickCallbacks.clear();

        while(callbacks.length()) {
            callbacks.takeFirst().call();
        }
    } else {
        QObject::customEvent(e);
    }
}
