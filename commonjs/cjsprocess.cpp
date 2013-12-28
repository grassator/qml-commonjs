#include "cjsprocess.h"
#include <stdio.h>

extern char **environ;

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
