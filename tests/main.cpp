#include <QtCore>
#include "commonjs.h"

#include <QtQml>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    CommonJS::registerQmlSingletonType("CommonJSTest");

    QQmlEngine *engine = new QQmlEngine;
    engine->setBaseUrl(QUrl::fromLocalFile(qApp->applicationDirPath()));
    qApp->connect(engine, SIGNAL(quit()), qApp, SLOT(quit()));

    QUrl url = QUrl::fromLocalFile(QFileInfo("qml/main.qml").absoluteFilePath());
    QQmlComponent mainQml(engine, url);
    mainQml.loadUrl(url);
    mainQml.create();

    // The only way component isn't ready is if something went wrong
    if(!mainQml.isReady() || mainQml.isError()) {
        qDebug() << mainQml.errors();
        return -1;
    }

    return app.exec();
}
