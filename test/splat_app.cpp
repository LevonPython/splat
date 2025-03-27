#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QQmlContext>
#include "QtSMSplat.h"
#include <QDir>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QtSMSplat::registerQtSMSplatQmlTypes();
    engine.addImportPath("qrc:/qml");
    auto smsplat = qobject_cast<QtSMSplat *>(QtSMSplat::createQtSMSplatSingleton(nullptr, nullptr));
    engine.addImageProvider("memory", smsplat->imageProvider());
    smsplat->initDatabase(QDir::currentPath() + "/test.db");
    const QUrl url(QStringLiteral("qrc:/qml/Splat/Test.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}