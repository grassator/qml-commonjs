#include "cjsprocess.h"
#include <stdio.h>

#ifndef Q_WS_WIN
#include <sys/types.h>
#include <unistd.h>
#endif

extern char **environ;

#ifndef Q_WS_WIN
int CJSProcess::getgid() const { return getgid();}
void CJSProcess::setgid(int gid) const { setgid(gid); }
int CJSProcess::getuid() const { return getuid(); }
void CJSProcess::setuid(int uid) const { setuid(uid); }
#endif

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
