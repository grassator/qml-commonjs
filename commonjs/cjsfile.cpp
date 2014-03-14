#include "cjsfile.h"
#include "sys/stat.h"
#include "QDebug"

CJSFile::CJSFile(CommonJS *main) : QObject(main), m_main(main)
{
    m_stat = main->scriptEngine()->evaluate(
                 "(function stat(path, callback){"
                 "  if(stat.native.doStat(path, callback) !== 0) throw new Error;"
                 "})");
    m_stat.setProperty("native", main->scriptEngine()->newQObject(this));
}

int CJSFile::doStat(QString path, QJSValue callback)
{
    Q_UNUSED(callback);
    struct ::stat result;
    int code = ::stat(path.toStdString().c_str(), &result);
    if(code != 0) {

    }
    return code;
}
